// SPDX-License-Identifier: UNLICENSED
pragma solidity >=0.6.0 <0.8.0;

import "./Universe.sol";

struct TicketInfo {
  uint256 d_prev_price;
  bool    d_used;
}

struct EventData {

    address payable d_admin;
    address payable d_organizer;
    
    uint8 d_creator_commission_percent;
    
    uint256 d_token_ask_num;
    
    // Mapping from owner to list of owned token IDs
    mapping(address => uint256[]) d_owner_tokens;
    
    // Mapping from token id to owner address
    mapping(uint256 => address payable) d_token_owner;
    
    // For transfers 
    mapping(uint256 => uint256) d_token_ask;

    // Array with all token ids, used for enumeration
    TicketInfo[] d_tickets;

}

library EventImpl {

    function getCostFor(EventData storage self,uint256 _numTickets) public view returns(uint256) {
      uint256 total_cost;//=0;
      uint256 bought;//=0;

      // We will buy 1 ticket at a time
      // If while buying, we do not find enough tickets, 
      // or we did not get enough money, we throw
      for(uint256 i=0;i<self.d_tickets.length && bought < _numTickets;++i) {
        if (self.d_token_owner[i] != address(0)) { continue; }

        // Ticket can be bought 
        total_cost+=self.d_tickets[i].d_prev_price;
        bought++;
      }

      require(bought == _numTickets, "Not enough tickets!");

      return total_cost;
    }

    function buy(EventData storage self,uint256 _numTickets,address payable to) public {
      uint256 total_cost=0;
      uint256 bought=0;

      // We will buy 1 ticket at a time
      // If while buying, we do not find enough tickets, 
      // or we did not get enough money, we throw
      for(uint256 i=0;i<self.d_tickets.length && bought < _numTickets;++i) {
        if (self.d_token_owner[i] != address(0)) { continue; }

        // Ticket can be bought 
        total_cost+=self.d_tickets[i].d_prev_price;
        self.d_owner_tokens[to].push(i);
        self.d_token_owner[i] = to;
        bought++;
  
      }

      require(bought == _numTickets, "Not enough tickets!");
      require(total_cost <= msg.value, "Cost is more than transaction value.");

      // Take admin cut
      uint256 commission = (msg.value / 100) * self.d_creator_commission_percent;
      self.d_admin.transfer(commission);

      Universe u = Universe(self.d_admin);
      u.addUserEvent(address(this),to);
    }

    function numSold(EventData storage self) public view returns(uint256) {
      uint256 numSoldCount=0;
      for(uint256 i=0;i<self.d_tickets.length;++i) {
        if (self.d_token_owner[i] != address(0)) { numSoldCount++;}
      }
      return numSoldCount;
    }

    function numUnSold(EventData storage self) public view returns(uint256) {
      uint256 numUnSoldCount=0;
      for(uint256 i=0;i<self.d_tickets.length;++i) {
        if (self.d_token_owner[i] == address(0)) { numUnSoldCount++; }
      }
      return numUnSoldCount;
    }

    function numUsed(EventData storage self) public view returns(uint256) {
      uint256 numUsedCount=0;
      for(uint256 i=0;i<self.d_tickets.length;++i) {
        if (self.d_tickets[i].d_used) { numUsedCount++; } 
      }
      return numUsedCount;
    }

    function numToBeUsed(EventData storage self) public view returns(uint256) {
      uint256 numToBeUsedCount=0;
      for(uint256 i=0;i<self.d_tickets.length;++i) {
        if (!self.d_tickets[i].d_used && self.d_token_owner[i] != address(0)) { numToBeUsedCount++; } 
      }
      return numToBeUsedCount;
    }

    function myTickets(EventData storage self, address owner) public view returns(uint256[] memory,uint256[] memory,bool[] memory,bool[] memory) {
        uint256[] storage tokens = self.d_owner_tokens[owner];
        uint256[] memory prices = new uint256[](tokens.length);
        bool[] memory for_sale = new bool[](tokens.length);
        bool[] memory used = new bool[](tokens.length);
        for(uint256 i=0;i<tokens.length;++i)
        {
            uint256 token=tokens[i];
            if (self.d_token_ask[token] != 0) {
                prices[i] = self.d_token_ask[token];
                for_sale[i] = true;
            } else {
                prices[i] = self.d_tickets[token].d_prev_price;
                for_sale[i] = false;
            }
            used[i] = self.d_tickets[token].d_used;
        }
        return (tokens,prices,for_sale,used);
    }

    function forSale(EventData storage self) public view returns(uint256,uint256[] memory,uint256[] memory) {
        uint256[] memory tokens = new uint256[](self.d_token_ask_num);
        uint256[] memory asks = new uint256[](self.d_token_ask_num);
        uint256 iter=0;
        for(uint256 i=0;i<self.d_tickets.length;++i)
        {
            if (self.d_token_ask[i] > 0) { 
                tokens[iter] = i;
                asks[iter] = self.d_token_ask[i];
                iter++;
            }
        }
        require(iter == self.d_token_ask_num);
        return (iter,tokens,asks);
    }

    function ticketUsed(EventData storage self, uint256 _tokenId) public view returns(bool) {
        return self.d_tickets[_tokenId].d_used;
    }

    function ticketVerificationCode(EventData storage self,uint256 _tokenId) public view returns(bytes32) {
        require(!ticketUsed(self,_tokenId), "Ticket already used!");
        return keccak256(abi.encodePacked(_tokenId,address(this)));
    }

    function splitSignature(bytes memory sig) internal pure returns (uint8 v, bytes32 r, bytes32 s)
    {
        require(sig.length == 65);
        assembly {
        // first 32 bytes, after the length prefix
        r := mload(add(sig, 32))
        // second 32 bytes
        s := mload(add(sig, 64))
        // final byte (first byte of the next 32 bytes)
        v := byte(0, mload(add(sig, 96)))
        }
        return (v, r, s);
    }

    function recover(bytes32 message, bytes memory signature) internal pure returns (address) {
        (uint8 v, bytes32 r, bytes32 s) = splitSignature(signature);
        return ecrecover(message,v, r, s);
    }

    
    function isOwnerSig(EventData storage self,uint256 _tokenId,bytes memory signature) public view returns(bool) {
        bytes memory prefix = "\x19Ethereum Signed Message:\n32";
        bytes32 prefixedHash = keccak256(abi.encodePacked(prefix, ticketVerificationCode(self,_tokenId)));
        return self.d_token_owner[_tokenId] == 
                recover(prefixedHash,signature);
    }

    function supportsInterface(bytes4 interfaceId) public pure returns (bool)
    { return interfaceId == 0x80ac58cd; }

}
