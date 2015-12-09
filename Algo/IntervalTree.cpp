#include "IntervalTree.h"

template<typename T, size_t N>
IntervalDimensionalTreeNode<T,N>::IntervalDimensionalTreeNode(const Ivals& arr, const ThisType * ptr):
	v_(new IntervalDimensionalTreeNode::IntervalTreeNodeVals()),
	std::enable_shared_from_this<ThisType>()
{
	BuildTree(arr,ptr);
}





//Points are equal
template<typename T, size_t N>
inline bool IntervalDimensionalTreeNode<T, N>::Equals(const Point & a, const Point & b) {
	for (size_t c = 0; c < N; ++c)
		if (a[c] != b[c])
			return false;
	return true;
}

template<typename T, size_t N>
inline int64_t IntervalDimensionalTreeNode<T, N>::GetChildAddress(const Point & x_center, const Point & p) {
	int64_t addr = 0;
	for (size_t c = 0; c < N; ++c)
		addr |= (int)(p[c] > x_center[c]) << c;
	return addr;
}

//Point lies in interval when all coordinates lie in corresponding coordinate interval
template<typename T, size_t N>
inline bool IntervalDimensionalTreeNode<T, N>::PointLiesInInterval(const Point & p, const Ival & i) {
	for (size_t c = 0; c < N; ++c)
		if (p[c] <= i.first[c] || p[c] >= i.second[c])
			return false;
	return true;
}

//bitwise 0 - <= on coordinate. 1 - else (>)
template<typename T, size_t N>
inline int64_t IntervalDimensionalTreeNode<T, N>::GetChildAddress(const Point & x_center, const Ival & iv) {
	int64_t addr = 0;
	for (size_t c = 0; c < N; ++c)
		addr |= (int)(iv.first[c] > x_center[c]) << c;
	return addr;
}

template<typename T, size_t N>
inline int64_t IntervalDimensionalTreeNode<T, N>::ChildAddressAndDiff(const Point & x_center, const Point & p, bool & equals) {
	int64_t addr = 0;
	equals = true;
	for (size_t c = 0; c < N; ++c)
	{
		addr |= (int)(p[c] > x_center[c]) << c;
		if (equals)
			equals = p[c] == x_center[c];
	}
	return addr;
}

template<typename T, size_t N>
inline int64_t IntervalDimensionalTreeNode<T, N>::ChildAddressAndDiff(const Point & x_center, const Ival & iv, bool & lies) {
	int64_t addr = 0;
	lies = true;
	for (size_t c = 0; c < N; ++c)
	{
		addr |= (int)(iv.first[c] > x_center[c]) << c;
		if (lies)
			lies = iv.first[c] <= x_center[c] && iv.second[c] >= x_center[c];
	}
	return addr;
}

//O(f(n)) = (O(n)+O(nlogn))+O(f(n/2)) = Sum(i = 1, i = log(n)+1, O(2^i*log(2^i) ) = nlog(n)^2
template<typename T,size_t N>
void IntervalDimensionalTreeNode<T, N>::BuildTree(const Ivals& arr, const ThisType *ptr) const
{
	//select median point
	std::vector<Point> medians;
	size_t s = arr.size();
	medians.reserve(s); //reserve space
	std::array<Ivals, 1 << N> children;

	/////////////// COPY TO CLASS ////////////
	this->v_->parent_ = ptr; //set parent
	//////////////////////////////////////////

	for (auto i = arr.begin(), s = arr.end(); i != s; ++i)
		medians.push_back(this->SecureCall(*i, IntervalDimensionalTreeNode<T, N>::Median));
	std::sort(medians.begin(), medians.end()); //n*log(n)
	//median of medians
	const Point& median = medians[arr.size() / 2];

	/////////////// COPY TO CLASS ///////////
	this->v_->p_center_ = median; //Set center point
	///////////////////////////////////////////
								  
	//each interval: contains this point (in this case - add this interval into 2*N vectors)
	bool median_lies_in_interval = true;
	int64_t child_address = 0;
	for (auto i = arr.begin(), s = arr.end(); i != s; ++i) {
		child_address = this->ChildAddressAndDiff(median, *i, median_lies_in_interval);
		if (median_lies_in_interval)
		{

			this->v_->intervals_.push_back(std::shared_ptr<Ival>(new Ival(*i)));
			const std::shared_ptr<Ival>& sp = this->v_->intervals_[this->v_->intervals_.size() - 1];
			std::weak_ptr<Ival> wp(sp);
			for (size_t di = 0; di < N; ++di) {
				this->v_->sorted_[2 * di].push_back(wp);
				this->v_->sorted_[2 * di + 1].push_back(wp);
			}
		}
		else
			children[child_address].push_back(*i);
	}
	//sort sorted_ by coordinate or reverse
	size_t di = 0;
	bool left = true;
	auto sort_by_xc_lambda = [&di,&left](const std::weak_ptr<Ival>& a, const std::weak_ptr<Ival>& b) {
		auto al = a.lock(), bl = b.lock();
		if(left)
			return al->first[di] < bl->first[di]; // sort by left
		else
			return al->second[di] < bl->second[di]; // sort by right
	};
	for (di = 0; di < N; ++di) {
		left = true;
		std::sort(this->v_->sorted_[2 * di].begin(), this->v_->sorted_[2 * di].end(), sort_by_xc_lambda);
		left = false;
		std::sort(this->v_->sorted_[2 * di + 1].rbegin(), this->v_->sorted_[2 * di + 1].rend(), sort_by_xc_lambda); //reverse sort by right

	}
	

	///////////////// RECURSION CALL ///////////////
	for (int64_t i = 0, s = 1 << N; i != s; ++i)
		if (children[i].size()) {
			this->v_->children_[i] = TPtr(new ThisType(children[i], this ));
		}
	
	///////////////// FULL TIME (OF THE) JOB, LOL ////////////
	// T(n) = (2dn+(2d+1)*nlogn)
}
template<typename T, size_t N>
inline size_t IntervalDimensionalTreeNode<T, N>::BinaryFind(const Point & p, size_t cx, bool right) {
	size_t s = this->v_->intervals_.size();
	size_t pos = s / 2, shift = s / 2;
	const std::vector<std::weak_ptr<Ival>> &arr = this->v_->sorted_[2 * cx + right];
	auto ok_lambda = [&pos, &arr, &right, &cx, &p]() {
		auto elem = arr[pos].lock();
		return (right&&elem->second[cx] >= p[cx]) || (!right&&elem->first[cx] <= p[cx]);
	};
	for (; shift; shift >>= 1) {
		bool ok = ok_lambda();
		pos += (ok ? 1 : -1)*shift;
	}
	return pos - (int)!ok_lambda();
}
template<typename T, size_t N>
IntervalDimensionalTreeNode<T, N>::IntervalDimensionalTreeNode() :
	v_(new IntervalDimensionalTreeNode<T, N>::IntervalTreeNodeVals()),
	std::enable_shared_from_this<ThisType>()
{

}

template<typename T, size_t N>
IntervalDimensionalTreeNode<T,N>::IntervalDimensionalTreeNode(const Ivals& arr) :
	v_(new IntervalDimensionalTreeNode<T,N>::IntervalTreeNodeVals()),
	std::enable_shared_from_this<ThisType>()
{
	
	BuildTree(arr, nullptr);

}

template<typename T, size_t N>
IntervalDimensionalTreeNode<T,N>::~IntervalDimensionalTreeNode()
{

}

//LET M - number of repoted results. N - number of intervals

//T(N) = T( O(M*log(M)) )+T( O((log(N)+M)^d) )
//O(N*log(N))
template<typename T, size_t N>
typename IntervalDimensionalTreeNode<T, N>::Ivals IntervalDimensionalTreeNode<T,N>::Find(const Point & point, char sort)
{
	Ivals candidates; 
	Ivals temp_candidates;

	auto copy_lambda = [](	const std::vector<std::shared_ptr<Ival>>::const_iterator &src_b,
							const std::vector<std::shared_ptr<Ival>>::const_iterator &src_e,
							Ivals &dst) {
		for (auto i = src_b; i != src_e; ++i)
			dst.push_back(*(*i));
	};
	auto copy_weak_lambda = [](
		const std::vector<std::weak_ptr<Ival>>::const_iterator &src_b,
		const std::vector<std::weak_ptr<Ival>>::const_iterator &src_e,
		Ivals &dst) {
		for (auto i = src_b; i != src_e; ++i)
			dst.push_back(*(i->lock()));
	};
	bool point_equals_median, first_filter_performed = false;
	int64_t next_addr = 0;
	TPtr p = this->shared_from_this();
	do {
		next_addr = ChildAddressAndDiff(p->v_->p_center_, point, point_equals_median);
		if (point_equals_median)
			copy_lambda(p->v_->intervals_.begin(), p->v_->intervals_.end(), temp_candidates);
		else {
			for (size_t di = 0; di < N && (!first_filter_performed || temp_candidates.size()); ++di)
			{
				//coordinate is equal
				if (point[di] == p->v_->p_center_[di]) {
					if (!first_filter_performed && !temp_candidates.size())
						copy_lambda(p->v_->intervals_.begin(), p->v_->intervals_.end(), temp_candidates);
				}
				//coordinate is not equal
				else {
					bool right = point[di] > p->v_->p_center_[di];
					if (first_filter_performed) // O(n)
					{
						for (auto it = temp_candidates.begin(), is = temp_candidates.end(); it != is;) {
							if (it->first[di]>point[di] || it->second[di]<point[di]) //if not lies in interval
								temp_candidates.erase(it);
							else
								++it;
						}
					}
					else { //O(log(N))
						size_t pos = BinaryFind(point, di, right);
						if (pos != -1) {
							copy_weak_lambda(
								p->v_->sorted_[2 * di + (int)right].begin(),
								p->v_->sorted_[2 * di + (int)right].begin() + pos + 1,
								temp_candidates);
							first_filter_performed = true;
						}
						
					}
				}
			}
		}
		if (p->v_->children_[next_addr]) {
			p = p->v_->children_[next_addr];
		}
		else
			p = nullptr;
		candidates.insert(candidates.begin(), temp_candidates.begin(), temp_candidates.end());
		temp_candidates.clear();
		first_filter_performed = false;
	} while (!point_equals_median&&p);
	

	//O(M*log(M))
	if (sort) {
		if (sort == 1)
			std::sort(
				candidates.begin(), candidates.end(),
				[this](const Ival& a, const Ival& b) { return SecureCall(a, Weight) < SecureCall(b, Weight); } );
		else 
			std::sort(
				candidates.rbegin(),candidates.rend(),
				[this](const Ival& a, const Ival& b) { return SecureCall(a, Weight) < SecureCall(b, Weight); } 	);
	}
		

	return candidates;
}

template<typename T, size_t N>
IntervalDimensionalTreeNode<T,N> IntervalDimensionalTreeNode<T, N>::Parse(std::istream & istr) {
	/*
	*	FILE FORMAT
	*	[....] - INTERVAL
	*	(....) - POINT
	*	{coordinate1,coordinate2...} - VALID POINT FORMAT
	*	[ ( ... ) , ( ... ) ] - VALID INTERVAL FORMAT
	*	FILE:
	*	[ ( ) , ( ) ]ANY_CHARS_HERE[ ( ) , ( ) ]ANY_CHARS_HERE[ ( ) , ( ) ]....
	*	SPACES CAN BE MULTIPLE - FILE FORMAT ALLOWS THAT
	*/

	// 0 - looking for interval start
	// 1 - looking for point start
	// 2 - parsing coordinate
	// 3 - looking for coordinate delimeter ( , )
	// 4 - closing point
	// 5 - looking for point delimeter ( , )
	// 6 - closing interval
	int scan_mode = 0;
	char ch;
	const char valid_chars[] = { '[','(',0,',', ')', ',', ']' };
	T xc;
	size_t n = 0;
	bool left = true;
	Point p;
	Ival i;
	Ivals is;
	while (!istr.eof()) {
		//skip space or any char (only if mode == 0)
		do
			ch = istr.get();
		while ( !istr.eof() && ( isspace(ch) || (scan_mode == 0 && ch != valid_chars[0]) ) );
		//skipped, stopped at first non-space char or '[' if mode is 0
		//decide what to do
		if (istr.eof())
			break;

		if (scan_mode == 2) { // if mode is 2 - parse type and put into array
			istr.unget(); //return char back
			istr >> xc; //parse coordinate
			p[n++] = xc; //set coordinate for point
			scan_mode = 3; //set scan mode to find delimeter
			if (n == N) //if point is ready - put into redying interval and close it
			{
				if (left)
					i.first = p;
				else
					i.second = p;

				left = !left; //change left
				scan_mode = 4; //point is ready - close it
				n = 0; //reset n
			}
		}
		else {
			//if met invalid char
			if (ch != valid_chars[scan_mode])
				throw std::exception(
					("Syntax Error [IntervalTree::Parse]: Found unexpected literal at position " +
						std::to_string(istr.tellg())).c_str());
			//if scan in "look for coordinate delimeter"-mode - set back into point parsing mode
			if (scan_mode == 3)
				scan_mode = 2;
			else if (scan_mode == 4 && left) {
				//if interval is ready - put into intervals and close it
				scan_mode = 6;
				//check and swap coordinates
				for (size_t di = 0; di < N; ++di)
					if (i.first[di]>i.second[di])
						std::swap(i.first[di], i.second[di]);
				is.push_back(i);
			}
			else if (scan_mode == 5)
				scan_mode = 1;
			//otherwise - forward mode
			else
				scan_mode = (scan_mode + 1) % 7;
		}

	}
	//okay, we have our intervals
	if (scan_mode != 0)
		throw std::exception("Syntax Error [IntervalTree::Parse]: Unfinished point or interval");
	return IntervalDimensionalTreeNode<T, N>::IntervalDimensionalTreeNode(is, nullptr);
}

template<typename T, size_t N>
template<typename Retn>
Retn IntervalDimensionalTreeNode<T,N>::SecureCall(const Ival &val,
	std::function<Retn(const Ival&)> F) const
{
	if (!F.operator bool())
		throw std::exception("Secure call failed: no function");
	return F(val);
}