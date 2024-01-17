#pragma once
#include "Present.h"

namespace fgo::audio_graph
{
	inline struct Presenter
	{
		inline static const UINT WM_SEND = WM_APP + 1;

		DWORD tid = 0;
		HANDLE thread = 0;

		BOOL m_isPlaying = FALSE;
		DWORD m_startTime = 0;
		int m_startFrame = 0;

		// �ȉ��̓��C���X���b�h���̏����ł��B

		//
		// ���������������s���܂��B
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ��n�����������s���܂��B
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			if (!thread) return FALSE;

//			::PostThreadMessage(tid, WM_QUIT, 0, 0);
			::TerminateThread(thread, 0);
			::CloseHandle(thread), thread = 0;

			return TRUE;
		}

		//
		// �T�u�X���b�h���J�n���܂��B
		//
		BOOL start()
		{
			MY_TRACE_FUNC("");

			if (thread) return FALSE;

			thread = ::CreateThread(0, 0, threadProc, this, 0, &tid);
			return !!thread;
		}

		//
		// �T�u�X���b�h�Ƀv���[���g�𑗂�܂��B
		//
		BOOL send(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const Servant::ProcState& proc_state)
		{
			MY_TRACE_FUNC("");

			if (fpip->audio_ch != 1 && fpip->audio_ch != 2)
				return FALSE;

			if (fpip->audio_n == 0)
				return FALSE;

			DWORD time = 0;

			if (isSameFrame(fp, fpip))
			{
				MY_TRACE(_T("�Đ����ł͂���܂���\n"));

				m_isPlaying = FALSE;
			}
			else
			{
				if (!proc_state.is_playing)
				{
					// �Đ����Ȃ̂Ƀt���[�������ݍ���Ȃ��Ƃ���
					// ���̃v���O�C������Ă΂�Ă���\��������̂ŉ������Ȃ��B
					return FALSE;
				}

				if (!m_isPlaying || m_startFrame >= fpip->frame)
				{
					MY_TRACE(_T("�Đ����J�n����܂���\n"));

					m_isPlaying = TRUE;
					m_startTime = ::timeGetTime();
					m_startFrame = fpip->frame;

					time = m_startTime;
				}
				else
				{
					MY_TRACE(_T("�Đ����ł�\n"));

					MY_TRACE_INT(fpip->frame);
					MY_TRACE_INT(m_startFrame);

					int offset = frame2ms(fp, fpip, fpip->frame - m_startFrame);
					MY_TRACE_INT(offset);

					time = m_startTime + offset;
					MY_TRACE_INT(time);
				}
			}

			// �����Ŋm�ۂ����������̓T�u�X���b�h�ŊJ������܂��B
			return ::PostThreadMessage(tid, WM_SEND, (WPARAM)new Present(fp, fpip, time), 0);
		}

		//
		// fpip->frame���ύX����Ă��Ȃ��ꍇ��TRUE��Ԃ��܂��B
		//
		static BOOL isSameFrame(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			return fp->exfunc->get_frame(fpip->editp) == fpip->frame;
		}

		//
		// �t���[���ԍ����~���b�ɕϊ����ĕԂ��܂��B
		//
		static int frame2ms(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, int frame)
		{
			AviUtl::FileInfo fi = {};
			fp->exfunc->get_file_info(fpip->editp, &fi);

			if (fi.video_rate != 0 && fi.video_scale != 0)
				return (int)(frame * 1000.0f * fi.video_scale / fi.video_rate);
			else
				return 0;
		}

		// �ȉ��̓T�u�X���b�h���̏����ł��B

		//
		// �X���b�h�֐�����Ă΂�܂��B
		//
		DWORD onThreadProc()
		{
			MY_TRACE_FUNC("");

			SubThread sub;

			return sub.messageLoop();
		}

		//
		// �X���b�h�֐��ł��B
		//
		static DWORD CALLBACK threadProc(LPVOID param)
		{
			MY_TRACE_FUNC("");

			Presenter* presenter = (Presenter*)param;

			return presenter->onThreadProc();
		}

		//
		// ���̃N���X�̓T�u�X���b�h�ł��B
		// �܂��A���C���X���b�h����v���[���g���󂯎��܂��B
		// ���ɂ��̃v���[���g�̒��g(����)�����L�������ɏ������݂܂��B
		// �Ō�ɃN���C�A���g�v���Z�X�Ƀ��b�Z�[�W�𑗐M���A�ĕ`��𑣂��܂��B
		//
		struct SubThread
		{
			Mutex mutex;
			SimpleFileMapping fileMapping;

			//
			// �R���X�g���N�^�ł��B
			//
			SubThread()
				: mutex(0, FALSE, FormatText(Share::AudioGraph::MutexFormat, hive.hostWindow))
				, fileMapping(sizeof(Share::AudioGraph::Volume), FormatText(Share::AudioGraph::FileMappingFormat, hive.hostWindow))
			{
				MY_TRACE_FUNC("");
			}

			//
			// �f�X�g���N�^�ł��B
			//
			~SubThread()
			{
				MY_TRACE_FUNC("");
			}

			//
			// ���b�Z�[�W���[�v�ł��B
			//
			DWORD messageLoop()
			{
				MSG msg = {};
				while (::GetMessage(&msg, 0, 0, 0))
				{
					if (!msg.hwnd)
					{
						switch (msg.message)
						{
						case WM_SEND:
							{
								// �N���C�A���g�v���Z�X�ɉ��ʂ𑗐M���܂��B
								onSend((Present*)msg.wParam);

								break;
							}
						case WM_QUIT:
							{
								// �X���b�h���I�����܂��B
								return 0;
							}
						}
					}

					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}

				return 0;
			}

			//
			// �N���C�A���g�v���Z�X�ɉ��ʂ𑗐M���܂��B
			//
			void onSend(Present* present)
			{
//				MY_TRACE_FUNC("");

				// ���ʂ��Z�o���܂��B
				present->calc();

				{
					// �Z�o���ꂽ���ʂ����L�������ɏ������݂܂��B

//					Synchronizer sync(mutex);
					auto shared = (Share::AudioGraph::Volume*)fileMapping.getBuffer();

					*shared = present->volume;
				}

				// �g�p�ς݂̃��������J�����܂��B
				delete present;

				// �N���C�A���g�v���Z�X�ɍĕ`��𑣂��܂��B
				::SendMessage(hive.clientWindow, Share::AudioGraph::Message::Redraw, 0, 0);
			}
		};
	} presenter;
}
