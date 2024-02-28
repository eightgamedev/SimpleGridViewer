# pragma once
# include "gridcell/GuiGridAxis.hpp"

using Coord = int32;

GuiGridAxis::Inner::Inner() : sep(), width(1, Coord(0)) {}

GuiGridAxis::Inner::Inner(Array<Coord> initialWidthList)
{
	width = std::move(initialWidthList);
	recalc();
}

size_t GuiGridAxis::Inner::count() const { return width.size(); }
Coord GuiGridAxis::Inner::fullWidth() const { return sep.back(); }

size_t GuiGridAxis::Inner::lbound(Coord x) const
{
	return (std::upper_bound(sep.begin(), sep.end(), x) - sep.begin()) - 1;
}
void GuiGridAxis::Inner::recalc()
{
	sep.resize(width.size() + 1);
	sep[0] = Coord(0);
	for (size_t i = 0; i < width.size(); i++) sep[i + 1] = sep[i] + width[i];
}

GuiGridAxis::GuiGridAxis()
{
	countSum = { 0 };
	widthSum = { Coord(0) };
}

GuiGridAxis::GuiGridAxis(Array<Coord> init)
{
	for (size_t s = 0; s < init.size(); s += B)
	{
		m_inner.emplace_back(Inner(Array(init.begin() + s, init.begin() + Min(s + B, init.size()))));
	}
	recalcOverBlocks();
}

void GuiGridAxis::recalcOverBlocks()
{
	countSum.resize(m_inner.size() + 1);
	for (size_t i = 0; i < m_inner.size(); i++) countSum[i + 1] = countSum[i] + m_inner[i].count();
	widthSum.resize(m_inner.size() + 1);
	for (size_t i = 0; i < m_inner.size(); i++) widthSum[i + 1] = widthSum[i] + m_inner[i].fullWidth();
}

size_t GuiGridAxis::size() const { return countSum.back(); }

Coord GuiGridAxis::totalWidth() const { return widthSum.back(); }

bool GuiGridAxis::isEmpty() const { return size() == 0; }

void GuiGridAxis::clear() { (*this) = GuiGridAxis(); }

// lower bound
size_t GuiGridAxis::posToIndex(Coord x) const
{
	size_t blockIndex = (std::upper_bound(widthSum.begin(), widthSum.end(), x) - widthSum.begin()) - 1;
	size_t inBlockIndex = m_inner[blockIndex].lbound(x - widthSum[blockIndex]);
	return countSum[blockIndex] + inBlockIndex;
}

Coord GuiGridAxis::indexToPos(size_t at) const
{
	if (at == size()) return totalWidth();
	size_t blockIndex = (std::upper_bound(countSum.begin(), countSum.end(), at) - countSum.begin()) - 1;
	Coord inBlockPos = m_inner[blockIndex].sep[at - countSum[blockIndex]];
	return widthSum[blockIndex] + inBlockPos;
}

Coord GuiGridAxis::getWidth(size_t at) const
{
	size_t blockIndex = (std::upper_bound(countSum.begin(), countSum.end(), at) - countSum.begin()) - 1;
	return widthSum[blockIndex] + m_inner[blockIndex].width[at - countSum[blockIndex]];
}

// ( left pos, width )
std::pair<Coord, Coord> GuiGridAxis::getCellRange(size_t at) const
{
	size_t blockIndex = (std::upper_bound(countSum.begin(), countSum.end(), at) - countSum.begin()) - 1;
	Coord inBlockPos = m_inner[blockIndex].sep[at - countSum[blockIndex]];
	Coord width = m_inner[blockIndex].width[at - countSum[blockIndex]];
	return { widthSum[blockIndex] + inBlockPos, width };
}

Array<Coord> GuiGridAxis::getWidthArray() const
{
	Array<Coord> res(size());
	auto i = res.begin();
	for (auto& b : m_inner) {
		i = std::copy(b.width.begin(), b.width.end(), i);
	}
	return res;
}

void GuiGridAxis::insert(size_t at, Coord width)
{
	// 空の場合
	if (size() == 0) {
		m_inner.push_back(Array<Coord>(1, width));
	}
	// 空でない場合は、存在するブロックに列を追加する
	else {
		size_t blockIndex = (std::upper_bound(countSum.begin(), countSum.end(), at) - countSum.begin()) - 1;
		if (blockIndex == m_inner.size()) blockIndex--;
		m_inner[blockIndex].width.insert(m_inner[blockIndex].width.begin() + (at - countSum[blockIndex]), width);
		m_inner[blockIndex].recalc();

		// ブロックのサイズが 2 * B 以上になったら分割
		if (m_inner[blockIndex].count() >= 2 * B) {
			auto base = std::move(m_inner[blockIndex].width);
			m_inner.emplace(m_inner.begin() + (blockIndex + 1), Array<Coord>(base.begin() + B, base.end()));
			m_inner[blockIndex + 1].recalc();
			m_inner[blockIndex] = Inner(Array<Coord>(base.begin(), base.begin() + B));
			m_inner[blockIndex].recalc();
		}
	}

	recalcOverBlocks();
}

void GuiGridAxis::erase(size_t at)
{
	size_t blockIndex = (std::upper_bound(countSum.begin(), countSum.end(), at) - countSum.begin()) - 1;
	m_inner[blockIndex].width.erase(m_inner[blockIndex].width.begin() + (at - countSum[blockIndex]));
	m_inner[blockIndex].recalc();

	// 隣接するブロックのサイズが B 以下になったら合体

	auto merge = [this](size_t block) {
		{
			auto& dest = m_inner[block].width;
			auto& src = m_inner[block + 1].width;
			std::copy(src.begin(), src.end(), std::back_inserter(dest));
		}
		m_inner[block].recalc();
		m_inner.erase(m_inner.begin() + (block + 1));
		};

	if (blockIndex != 0 && m_inner[blockIndex - 1].count() + m_inner[blockIndex].count() <= B) {
		merge(blockIndex - 1);
		blockIndex -= 1;
	}
	if (blockIndex != m_inner.size() - 1 && m_inner[blockIndex + 1].count() + m_inner[blockIndex].count() <= B) {
		merge(blockIndex);
	}

	recalcOverBlocks();
}

// サボり
void GuiGridAxis::setWidth(size_t at, Coord newWidth)
{
	erase(at);
	insert(at, newWidth);
}
