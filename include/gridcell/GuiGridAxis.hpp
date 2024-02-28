# pragma once

class GuiGridAxis {
public:
	using Coord = int32;

	// 実際は B=100 とか B=1000 くらいだと思う
	static inline const size_t B = 2;

	struct Inner {
		Array<Coord> sep;
		Array<Coord> width;

		Inner();

		Inner(Array<Coord> initialWidthList);

		size_t count() const;

		Coord fullWidth() const;

		size_t lbound(Coord x) const;

		void recalc();
	};

	Array<Inner> m_inner;

	// 累積和
	// サイズは (ブロック数+1) 、 countSum[0] = 0
	Array<size_t> countSum;

	// 累積和
	// サイズは (ブロック数+1) 、 widthSum[0] = 0
	Array<Coord> widthSum;

	GuiGridAxis();

	GuiGridAxis(Array<Coord> init);

	void recalcOverBlocks();

	size_t size() const;

	Coord totalWidth() const;

	bool isEmpty() const;

	void clear();

	// lower bound
	size_t posToIndex(Coord x) const;

	Coord indexToPos(size_t at) const;

	Coord getWidth(size_t at) const;

	// ( left pos, width )
	std::pair<Coord, Coord> getCellRange(size_t at) const;

	Array<Coord> getWidthArray() const;

	void insert(size_t at, Coord width);

	void erase(size_t at);

	// サボり
	void setWidth(size_t at, Coord newWidth);

};
