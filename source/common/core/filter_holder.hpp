#pragma once

#define MY_TRACE_FILTER_HOLDER(filter) \
do { \
	MY_TRACE(#filter _T(" = {}\n"), filter.get_filter_index()); \
} while (false)

namespace apn
{
	struct FilterHolder
	{
		ObjectHolder object;
		int32_t filter_index = -1;
		ExEdit::Filter* filter = nullptr;

		FilterHolder()
		{
		}

		FilterHolder(const ObjectHolder& object, int32_t filter_index)
			: object(object)
		{
			if (this->object.is_valid())
			{
				this->filter_index = filter_index;
				if (this->filter_index >= 0)
				{
					this->filter = magi.exin.get_filter(this->object.get_object(), this->filter_index);
					if (!this->filter)
						this->filter_index = -1;
				}
			}
		}

		const ObjectHolder& get_object() const
		{
			return object;
		}

		int32_t get_filter_index() const
		{
			return filter_index;
		}

		ExEdit::Filter* get_filter() const
		{
			return filter;
		}

		BOOL is_valid() const
		{
			if (filter_index < 0) return FALSE;
			if (!filter) return FALSE;
			return TRUE;
		}

		bool operator==(const FilterHolder& x) const
		{
			return object == x.object && filter_index == x.filter_index && filter == x.filter;
		}

		bool operator!=(const FilterHolder& x) const
		{
			return !operator==(x);
		}

		BOOL is_moveable() const
		{
			auto id = object.get_object()->filter_param[filter_index].id;
			switch (id)
			{
			case 0x00: // 動画ファイル
			case 0x01: // 画像ファイル
			case 0x02: // 音声ファイル
			case 0x03: // テキスト
			case 0x04: // 図形
			case 0x05: // フレームバッファ
			case 0x06: // 音声波形
			case 0x07: // シーン
			case 0x08: // シーン(音声)
			case 0x09: // 直前オブジェクト
			case 0x0A: // 標準描画
			case 0x0B: // 拡張描画
			case 0x0C: // 標準再生
			case 0x0D: // パーティクル出力
			case 0x50: // カスタムオブジェクト
			case 0x5D: // 時間制御
			case 0x5E: // グループ制御
			case 0x5F: // カメラ制御
				{
					return FALSE;
				}
			}
			return TRUE;
		}

		LPCSTR get_name() const
		{
			auto object_index = get_object().get_object_index();
			auto midpt_leader = get_object().get_object()->index_midpt_leader;
			MY_TRACE_INT(midpt_leader);
			if (midpt_leader >= 0) object_index = midpt_leader;

			ObjectHolder object(object_index);

			auto id = object.get_object()->filter_param[filter_index].id;
			if (id == 79) // アニメーション効果
			{
				auto exdata = magi.exin.get_exdata(object.get_object(), filter_index);
				auto name = (LPCSTR)(exdata + 0x04);
				if (!name[0])
				{
					auto type = *(WORD*)(exdata + 0);
					MY_TRACE_HEX(type);

					auto filter = *(WORD*)(exdata + 2);
					MY_TRACE_HEX(filter);

					switch (type)
					{
					case 0x00: name = "Trembling"; break;
					case 0x01: name = "Pendulum"; break;
					case 0x02: name = "Bounce"; break;
					case 0x03: name = "Scale coordinate(as individual object)"; break;
					case 0x04: name = "Appeared from off-screen"; break;
					case 0x05: name = "Appeared from random direction"; break;
					case 0x06: name = "Appear while scailing"; break;
					case 0x07: name = "Appear while falling at random interval"; break;
					case 0x08: name = "Appeare with bounce"; break;
					case 0x09: name = "Appear spreading"; break;
					case 0x0A: name = "Appear while rising up"; break;
					case 0x0B: name = "Appear from nowhere"; break;
					case 0x0C: name = "Repetitive"; break;
					case 0x0D: name = "Rotation of the coordinate (individual objects)"; break;
					case 0x0E: name = "Cube (camera control)"; break;
					case 0x0F: name = "Sphere (camera control)"; break;
					case 0x10: name = "Shatter"; break;
					case 0x11: name = "Blinking"; break;
					case 0x12: name = "Appear while blinking"; break;
					case 0x13: name = "Simple deformation"; break;
					case 0x14: name = "Simple deformation (camera control)"; break;
					case 0x15: name = "Reel rotation"; break;
					case 0x16: name = "Kaleidoscope"; break;
					case 0x17: name = "Circular arrangement"; break;
					case 0x18: name = "Random location"; break;
					default: name = "Animation effect"; break;
					}
				}
				return name;
			}
			else
			{
				return filter->name;
			}
		}
	};
}
