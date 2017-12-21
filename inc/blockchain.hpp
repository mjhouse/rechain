#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#include <string>
#include <vector>

class Block;
class KeyPair;

class BlockChain {
	private:
		std::vector<Block> chain;

	public:
		BlockChain(){}
		BlockChain( std::string fn ){ this->load(fn); }

		~BlockChain(){}

		void publish( Block new_block, KeyPair keys );
		Block& get( int i );

		void print();

		bool load( std::string fn );

		bool save( std::string fn );

		template<class Archive>
		void serialize(Archive& archive) {
			archive(CEREAL_NVP(this->chain));
		}

};

#endif
