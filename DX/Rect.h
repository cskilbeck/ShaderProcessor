//////////////////////////////////////////////////////////////////////

struct RectF;

struct Rect2D: RECT
{
	////////////////////////////////////////////////////////////////////// 

	Rect2D()
	{
	}

	//////////////////////////////////////////////////////////////////////

	explicit Rect2D(RectF const &rf);

	//////////////////////////////////////////////////////////////////////

	Rect2D(Point2D topLeft, Size2D size)
	{
		left = topLeft.x;
		top = topLeft.y;
		right = left + size.Width();
		bottom = top + size.Height();
	}

	//////////////////////////////////////////////////////////////////////

	Rect2D(Vec2 topLeft, Vec2 size)
	{
		left = (LONG)topLeft.x;
		top = (LONG)topLeft.y;
		right = (LONG)(left + size.x);
		bottom = (LONG)(top + size.y);
	}

	//////////////////////////////////////////////////////////////////////

	Rect2D(int x, int y, int width, int height)
	{
		left = x;
		top = y;
		right = x + width;
		bottom = y + height;
	}

	//////////////////////////////////////////////////////////////////////

	bool Contains(Point2D p) const
	{
		return p.x >= left && p.x < right && p.y >= top && p.y < bottom;
	}

	//////////////////////////////////////////////////////////////////////

	int Width() const
	{
		return right - left;
	}

	//////////////////////////////////////////////////////////////////////

	int Height() const
	{
		return bottom - top;
	}

	//////////////////////////////////////////////////////////////////////

	Point2D TopLeft() const
	{
		return Point2D(left, top);
	}

	//////////////////////////////////////////////////////////////////////

	Point2D TopRight() const
	{
		return Point2D(right, top);
	}

	//////////////////////////////////////////////////////////////////////

	Point2D BottomLeft() const
	{
		return Point2D(left, bottom);
	}

	//////////////////////////////////////////////////////////////////////

	Point2D BottomRight() const
	{
		return Point2D(right, bottom);
	}

	//////////////////////////////////////////////////////////////////////

	Size2D GetSize() const
	{
		return Size2D(Width(), Height());
	}

	//////////////////////////////////////////////////////////////////////

	Size2D HalfSize() const
	{
		return Size2D(Width() / 2, Height() / 2);
	}

	//////////////////////////////////////////////////////////////////////

	Point2D MidPoint() const
	{
		return TopLeft() + HalfSize();
	}

	//////////////////////////////////////////////////////////////////////

	Vec2 FSize() const
	{
		return Vec2((float)Width(), (float)Height());
	}

	//////////////////////////////////////////////////////////////////////

	void MoveTo(Point2D pos)
	{
		right = pos.x + Width();
		bottom = pos.y + Height();
		left = pos.x;
		top = pos.y;
	}

	//////////////////////////////////////////////////////////////////////

	void Translate(Point2D offset)
	{
		left += offset.x;
		top += offset.y;
		right += offset.x;
		bottom += offset.y;
	}

	//////////////////////////////////////////////////////////////////////

	void Resize(Size2D newSize)
	{
		right = left + newSize.Width();
		bottom = top + newSize.Height();
	}
};

//////////////////////////////////////////////////////////////////////

struct RectF
{
	//////////////////////////////////////////////////////////////////////

	Vec2 topLeft;
	Vec2 bottomRight;

	//////////////////////////////////////////////////////////////////////

	RectF()
	{
	}

	//////////////////////////////////////////////////////////////////////

	RectF(Vec2 const &topLeft, Vec2 const &bottomRight)
		: topLeft(topLeft)
		, bottomRight(bottomRight)
	{
	}

	//////////////////////////////////////////////////////////////////////

	explicit RectF(Rect2D const &r)
		: topLeft((float)r.left, (float)r.top)
		, bottomRight((float)r.top, (float)r.bottom)
	{
	}

	//////////////////////////////////////////////////////////////////////

	void Set(Vec2 const &topLeft, Vec2 const &bottomRight)
	{
		this->topLeft = topLeft;
		this->bottomRight = bottomRight;
	}

	//////////////////////////////////////////////////////////////////////

	float Width() const
	{
		return bottomRight.x - topLeft.x;
	}

	//////////////////////////////////////////////////////////////////////

	float Height() const
	{
		return bottomRight.y - topLeft.y;
	}

	//////////////////////////////////////////////////////////////////////

	Vec2 Size() const
	{
		return bottomRight - topLeft;
	}

	//////////////////////////////////////////////////////////////////////

	Vec2 HalfSize() const
	{
		return Size() / 2;
	}

	//////////////////////////////////////////////////////////////////////

	Vec2 const &TopLeft() const
	{
		return topLeft;
	}

	//////////////////////////////////////////////////////////////////////

	Vec2 const &BottomRight() const
	{
		return bottomRight;
	}

	//////////////////////////////////////////////////////////////////////

	Vec2 TopRight() const
	{
		return Vec2(bottomRight.x, topLeft.y);
	}

	//////////////////////////////////////////////////////////////////////

	Vec2 BottomLeft() const
	{
		return Vec2(topLeft.x, bottomRight.y);
	}

	//////////////////////////////////////////////////////////////////////

	Vec2 MidPoint() const
	{
		return topLeft + Size() * 0.5f;
	}

	//////////////////////////////////////////////////////////////////////

	bool Contains(Vec2 const &p) const
	{
		return p.x >= topLeft.x && p.x < bottomRight.x && p.y >= topLeft.y && p.y < bottomRight.y;
	}

	//////////////////////////////////////////////////////////////////////

	void Resize(Vec2 const &size)
	{
		bottomRight = topLeft + size;
	}

	//////////////////////////////////////////////////////////////////////

	void MoveTo(Vec2 const &newtopLeft)
	{
		Vec2 delta = newtopLeft - topLeft;
		bottomRight += delta;
		topLeft = newtopLeft;
	}

	//////////////////////////////////////////////////////////////////////

	void Translate(Vec2 const &offset)
	{
		topLeft += offset;
		bottomRight += offset;
	}

	//////////////////////////////////////////////////////////////////////

	bool operator == (RectF const &b)
	{
		return topLeft == b.topLeft && bottomRight == b.bottomRight;
	}

	string ToString() const
	{
		return Format("(%f,%f)-(%f,%f)", TopLeft().x, TopLeft().y, BottomRight().x, BottomRight().y);
	}

	RectF &operator = (Rect2D const &r)
	{
		topLeft = r.TopLeft();
		bottomRight = r.BottomRight();
		return *this;
	}
};

//////////////////////////////////////////////////////////////////////

inline Rect2D::Rect2D(RectF const &rf)
{
	left = (LONG)rf.topLeft.x;
	top = (LONG)rf.topLeft.y;
	right = (LONG)rf.bottomRight.x;
	bottom = (LONG)rf.bottomRight.y;
}

//////////////////////////////////////////////////////////////////////


