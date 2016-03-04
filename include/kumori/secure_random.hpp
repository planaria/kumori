#pragma once

namespace kumori
{

	class secure_random
	{
	public:

		secure_random()
		{
			std::array<unsigned int, std::mt19937::state_size> seed;

			std::random_device device;
			std::generate(seed.begin(), seed.end(), std::ref(device));

			std::seed_seq seq(seed.begin(), seed.end());
			generator_.seed(seq);
		}

		boost::uuids::uuid uuid()
		{
			boost::uuids::basic_random_generator<std::mt19937> gen(generator_);
			return gen();
		}

		static secure_random& get()
		{
			thread_local secure_random singleton;
			return singleton;
		}

	private:

		std::mt19937 generator_;

	};

}
