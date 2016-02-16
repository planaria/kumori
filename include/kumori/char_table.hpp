#pragma once

namespace kumori
{

	class char_table : public std::unary_function<char, bool>
	{
	public:

		template <class Pred>
		explicit char_table(Pred pred)
		{
			for (int i = 0; i < CHAR_MAX; ++i)
				table_.set(i, pred(static_cast<char>(i)));
		}

		bool operator ()(char c) const
		{
			return table_.test(static_cast<std::size_t>(c));
		}

	private:

		std::bitset<1 << CHAR_BIT> table_;

	};

}
