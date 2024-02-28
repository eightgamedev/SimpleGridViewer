// https://github.com/sthairno/SasaGUI
// 一部メンバ関数をpublicに変更
// ScrollBarのThumbThicknessを6から12に変更

#include <Siv3D.hpp> // OpenSiv3D v0.6.7

namespace SasaGUI
{
	namespace detail
	{
		static constexpr StringView DefaultAddonName = U"SasaGUI";

		class WindowImpl;

		class GUIImpl;
	}

	enum class WindowFlag : int32
	{
		/// <summary>
		/// 通常のウィンドウ
		/// </summary>
		None = 0,
		/// <summary>
		/// タイトルバーを表示しない
		/// </summary>
		NoTitlebar = 1 << 0,
		/// <summary>
		/// サイズ変更をしない
		/// </summary>
		NoResize = 1 << 1,
		/// <summary>
		/// 移動をしない
		/// </summary>
		NoMove = 1 << 2,
		/// <summary>
		/// 背景を表示しない
		/// </summary>
		NoBackground = 1 << 3,
		/// <summary>
		/// スクロールバーを表示しない
		/// </summary>
		NoScrollbar = 1 << 4,
		/// <summary>
		/// 自動リサイズ
		/// </summary>
		AutoResize = 1 << 5,
		/// <summary>
		/// 常に手前に表示
		/// </summary>
		AlwaysForeground = 1 << 6,
		/// <summary>
		/// フォーカスしない
		/// </summary>
		NoFocus = 1 << 7,
		/// <summary>
		/// ウィンドウを隠す
		/// </summary>
		Hide = 1 << 8,
		/// <summary>
		/// 無効
		/// </summary>
		Disable = 1 << 9,

		Debug = 1 << 31
	};
	DEFINE_BITMASK_OPERATORS(WindowFlag);

	struct Window
	{
		String displayName;

		Rect rect;

		WindowFlag flags;

		Font font;

		Size maxSize{ std::numeric_limits<Size::value_type>::max(), std::numeric_limits<Size::value_type>::max() };

		int32 space = 5;

		int32 padding = 10;

		bool defined = false;

		Rect lineRect{ padding, padding, 0, 0 };

		bool requestMoveToFront = false;

		bool sameLine = false;

		bool hasTitlebar() const
		{
			return
				not (flags & WindowFlag::NoBackground) &&
				not (flags & WindowFlag::NoTitlebar);
		}

		bool isResizable() const
		{
			return
				not (flags & WindowFlag::NoResize) &&
				not (flags & WindowFlag::AutoResize);
		}

		bool isMovable() const
		{
			return
				hasTitlebar() &&
				not (flags & WindowFlag::NoMove) &&
				not (flags & WindowFlag::AutoResize);
		}
	};

	class IControl
	{
	public:

		virtual Size computeSize() const = 0;

		virtual void update(Rect rect, Optional<Vec2> cursorPos) = 0;

		virtual void draw() const = 0;

		virtual ~IControl() { };
	};

	class GUIManager
	{
	public:

		GUIManager();

	public:

		const Font& getFont() const;

		void setFont(const Font& newFont);

		// Frame

		void frameBegin();

		void frameEnd();

		// Window

		template<class FuncType>
		inline void window(const StringView name, FuncType&& func)
		{
			windowBegin(name);
			func(*this);
			windowEnd();
		}

		template<class FuncType>
		inline void window(const StringView name, WindowFlag flags, FuncType&& func)
		{
			windowBegin(name, flags);
			func(*this);
			windowEnd();
		}

		void windowBegin(const StringView name, WindowFlag flags = WindowFlag::None);

		void windowEnd();

		void setWindowSize(Size size);

		void setWindowDisplayName(const StringView name);

		inline void setWindowPos(Point pos) { setWindowPos(pos, { 0.0, 0.0 }); }

		inline void setWindowPos(Arg::topLeft_<Vec2> topLeft) { setWindowPos(*topLeft, { 0.0, 0.0 }); }

		inline void setWindowPos(Arg::topCenter_<Vec2> topCenter) { setWindowPos(*topCenter, { 0.5, 0.0 }); }

		inline void setWindowPos(Arg::topRight_<Vec2> topRight) { setWindowPos(*topRight, { 1.0, 0.0 }); }

		inline void setWindowPos(Arg::leftCenter_<Vec2> leftCenter) { setWindowPos(*leftCenter, { 0.0, 0.5 }); }

		inline void setWindowPos(Arg::center_<Vec2> center) { setWindowPos(*center, { 0.5, 0.5 }); }

		inline void setWindowPos(Arg::rightCenter_<Vec2> rightCenter) { setWindowPos(*rightCenter, { 1.0, 0.5 }); }

		inline void setWindowPos(Arg::bottomLeft_<Vec2> bottomLeft) { setWindowPos(*bottomLeft, { 1.0, 1.0 }); }

		inline void setWindowPos(Arg::bottomCenter_<Vec2> bottomCenter) { setWindowPos(*bottomCenter, { 0.5, 1.0 }); }

		inline void setWindowPos(Arg::bottomRight_<Vec2> bottomRight) { setWindowPos(*bottomRight, { 0.0, 1.0 }); }

		const Window& getDefaultWindow() const;

		const Window& getCurrentWindow() const;

		// Controls

		void sameLine();

		void dummy(Size size);

		bool button(const StringView label);

		TextEditState& simpleTextBox(const StringView id, double width = 200, const Optional<size_t>& maxChars = unspecified);

		void label(const StringView text, ColorF color = Palette::Black);

		void image(Texture texture, ColorF diffuse = Palette::White);

		void image(TextureRegion texture, ColorF diffuse = Palette::White);

		bool checkbox(bool& checked, const StringView label = U"");

		bool radiobutton(bool selected, const StringView label = U"");

		template<class T, class U = T>
		bool radiobutton(T& target, const U& value, const StringView label = U"")
		{
			bool clicked = radiobutton(target == value, label);
			if (clicked)
			{
				target = value;
			}
			return clicked;
		}

		size_t& tab(const StringView id, Array<String> tabNames);

		// void dropdown();

		bool simpleColorpicker(HSV& value);

		bool simpleSlider(double& value, double width = 120);

		// template<class T>
		// void spinbox(T& target, T step = static_cast<T>(1), T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max());

		// void split();

		void progressbar(double value, int32 width = 200);

		bool toggleSwitch(bool& value);

		void custom(std::shared_ptr<IControl> control);

	private:
		
		std::unique_ptr<detail::GUIImpl> m_impl;

		std::vector<detail::WindowImpl*> m_stack;

		detail::WindowImpl* m_defaultWindow;

		detail::WindowImpl& getDefaultWindowImpl() { return *m_defaultWindow; }

		detail::WindowImpl& getCurrentWindowImpl() { return *m_stack.back(); }

		void setWindowPos(Vec2 pos, Vec2 offset);

	public:

		~GUIManager();
	};

	bool RegisterAddon(const StringView name, int32 priority = 0);

	bool RegisterAddon(const StringView name, int32 updatePriority, int32 drawPriority);

	inline bool RegisterAddon(int32 priority = 0)
	{
		return RegisterAddon(detail::DefaultAddonName, priority);
	}

	inline bool RegisterAddon(int32 updatePriority, int32 drawPriority)
	{
		return RegisterAddon(detail::DefaultAddonName, updatePriority, drawPriority);
	}

	GUIManager& Get(const StringView name = detail::DefaultAddonName);

	enum class Orientation
	{
		Horizontal, Vertical
	};

	namespace Config
	{
		struct ScrollBar
		{
			// |          ______________     |
			// |  =======|______________|==  |
			// |                             |
			//    ^---------Track---------^
			//           ^-----Thumb----^

			constexpr static ColorF BackgroundColor{ 1, 0 };
			constexpr static ColorF TrackColor{ 0, 0 };
			constexpr static ColorF ThumbColor{ 0.4, 0.7 };
			constexpr static ColorF HoveredBackgroundColor{ 1, 1 };
			constexpr static ColorF HoveredTrackColor{ 0, 0.1 };
			constexpr static ColorF HoveredThumbColor{ 0.4, 1 };

			constexpr static int32 TrackMargin = 3;
			constexpr static int32 ThumbThickness = 12; // 6
			constexpr static int32 ThumbMinLength = 20;

			constexpr static Duration ScrollSmoothTime = 0.1s;
			constexpr static Duration LargeScrollDelay = 0.8s;
			constexpr static Duration LargeScrollInterval = 0.1s;
			constexpr static Duration FadeInDuration = 0.06s;
			constexpr static Duration FadeOutDuration = 0.5s;
			constexpr static Duration FadeOutDelay = 2s;
		};
	}

	class Delay
	{
	public:

		constexpr Delay(Duration duration) noexcept;

		bool update(bool in, double d = Scene::DeltaTime()) noexcept;

	private:

		double m_duration;

		double m_time;
	};

	struct Repeat
	{
	public:

		Repeat(Duration interval, Duration delay = 0s);

		bool update(bool in, double d = Scene::DeltaTime()) noexcept;

	private:

		double m_interval;

		double m_delay;

		bool m_first = true;

		bool m_delayed = false;

		double m_accumulation = 0.0;
	};

	class ScrollBar
	{
	public:

		using Config = Config::ScrollBar;

		constexpr static int32 Thickness = Config::TrackMargin * 2 + Config::ThumbThickness;
		constexpr static int32 MinLength = Config::TrackMargin * 2 + Config::ThumbMinLength + 10;
		constexpr static double ThumbRoundness = Config::ThumbThickness * 0.5;

		ScrollBar(Orientation orientation);

		const Orientation orientation;

		Rect rect() const;

		Rect getTrackRect() const;

		RectF getThumbRect() const;

		double minimum() const;

		double maximum() const;

		double value() const;

		double viewportSize() const;

		void updateLayout(Rect rect);
		void updateConstraints(double minimum, double maximum, double viewportSize);
		void show();
		void scroll(double delta, bool teleport = false);
		void moveTo(double value);
		void update(Optional<Vec2> cursorXYPos = Cursor::PosF(), double deltaTime = Scene::DeltaTime());
		void draw() const;

	private:

		// 制限&スクロール位置

		double m_minimum = 0.0;

		double m_maximum = 1.0;

		double m_value = 0.0;

		double m_viewportSize = 1.0;

		// レイアウト

		Rect m_rect;

		double m_trackLength;

		double m_thumbLength = 0.0; // 表示しないとき0

		// 状態

		bool m_thumbGrabbed = false;

		bool m_trackPressed = false;

		// スクロール状態

		double m_scrollTarget = 0.0;

		double m_scrollVelocity = 0.0;

		int32 m_largeScrollDirection = 0;

		double m_largeScrollTargetPos = 0.0;

		// アニメーション関連

		Repeat m_largeScrollTimer{ Config::LargeScrollInterval, Config::LargeScrollDelay };

		Transition m_colorTransition{ Config::FadeInDuration, Config::FadeOutDuration };

		Delay m_colorTransitionDelay{ Config::FadeOutDelay };

		// その他

		Optional<double> prevCursorPos;

		void updateThumbLength();


		inline double calculateThumbPos(double value) const
		{
			return (m_trackLength - m_thumbLength) * value / (m_maximum - m_minimum - m_viewportSize);
		}

		double getMainAxisValue(Vec2 input) const;
	};
}
