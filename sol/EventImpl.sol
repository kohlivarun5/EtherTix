// SPDX-License-Identifier: UNLICENSED
pragma solidity >=0.6.0 <0.8.0;

import "./Universe.sol";

struct TicketInfo {
  uint    d_prev_price;
  bool    d_used;
}

// We use uint24 as the unit for storing NUMBER of tickets
struct EventData {

    // These are packed together
    uint8 d_creator_commission_percent;
    uint24 d_token_ask_num;
    
    // These are packed
    address payable d_admin;
    address payable d_organizer;
    // bool d_mark_delete;
    
    // Mapping from owner to list of owned token IDs
    mapping(address => uint24[]) d_owner_tokens;
    
    // Mapping from token id to owner address
    mapping(uint24 => address payable) d_token_owner;
    
    // For transfers 
    mapping(uint24 => uint) d_token_ask;

    // Array with all token ids, used for enumeration
    TicketInfo[] d_tickets;

}

library EventImpl {

    function getCostFor(EventData storage self,uint24 _numTickets) public view returns(uint) {
      uint total_cost;
      uint24 bought;

      // We will buy 1 ticket at a time
      // If while buying, we do not find enough tickets, 
      // or we did not get enough money, we throw
      for(uint24 i=0;i<self.d_tickets.length && bought < _numTickets;++i) {
        if (self.d_token_owner[i] != address(0)) { continue; }

        // Ticket can be bought 
        total_cost+=self.d_tickets[i].d_prev_price;
        bought++;
      }

      require(bought == _numTickets, "Not enough tickets!");

      return total_cost;
    }

    function buy(EventData storage self,uint24 _numTickets,address payable to) public {
      uint  total_cost;
      uint24 bought;

      // We will buy 1 ticket at a time
      // If while buying, we do not find enough tickets, 
      // or we did not get enough money, we throw
      for(uint24 i=0;i<self.d_tickets.length && bought < _numTickets;++i) {
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
      uint  commission = (msg.value / 100) * self.d_creator_commission_percent;
      self.d_admin.transfer(commission);

      Universe u = Universe(self.d_admin);
      u.addUserEvent(address(this),to);
    }

    function numSold(EventData storage self) public view returns(uint24) {
      uint24 numSoldCount=0;
      for(uint24 i=0;i<self.d_tickets.length;++i) {
        if (self.d_token_owner[i] != address(0)) { numSoldCount++;}
      }
      return numSoldCount;
    }

    function numUnSold(EventData storage self) public view returns(uint24) {
      uint24 numUnSoldCount=0;
      for(uint24 i=0;i<self.d_tickets.length;++i) {
        if (self.d_token_owner[i] == address(0)) { numUnSoldCount++; }
      }
      return numUnSoldCount;
    }

    function numUsed(EventData storage self) public view returns(uint24) {
      uint24 numUsedCount=0;
      for(uint24 i=0;i<self.d_tickets.length;++i) {
        if (self.d_tickets[i].d_used) { numUsedCount++; } 
      }
      return numUsedCount;
    }

    function numToBeUsed(EventData storage self) public view returns(uint24) {
      uint24 numToBeUsedCount=0;
      for(uint24 i=0;i<self.d_tickets.length;++i) {
        if (!self.d_tickets[i].d_used && self.d_token_owner[i] != address(0)) { numToBeUsedCount++; } 
      }
      return numToBeUsedCount;
    }

    function myTickets(EventData storage self, address owner) public view returns(uint24[] memory,uint[] memory,bool[] memory,bool[] memory) {
        uint24[] storage tokens = self.d_owner_tokens[owner];
        uint[] memory prices = new uint[](tokens.length);
        bool[] memory for_sale = new bool[](tokens.length);
        bool[] memory used = new bool[](tokens.length);
        for(uint24 i=0;i<tokens.length;++i)
        {
            uint24 token=tokens[i];
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

    function forSale(EventData storage self) public view returns(uint24,uint24[] memory,uint[] memory) {
        uint24[] memory tokens = new uint24[](self.d_token_ask_num);
        uint[] memory asks = new uint[](self.d_token_ask_num);
        uint24 iter=0;
        for(uint24 i=0;i<self.d_tickets.length;++i)
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

    function ticketUsed(EventData storage self, uint24 _tokenId) public view returns(bool) {
        return self.d_tickets[_tokenId].d_used;
    }

    function ticketVerificationCode(EventData storage self,uint24 _tokenId) public view returns(bytes32) {
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

    
    function isOwnerSig(EventData storage self,uint24 _tokenId,bytes memory signature) public view returns(bool) {
        bytes memory prefix = "\x19Ethereum Signed Message:\n32";
        bytes32 prefixedHash = keccak256(abi.encodePacked(prefix, ticketVerificationCode(self,_tokenId)));
        return self.d_token_owner[_tokenId] == 
                recover(prefixedHash,signature);
    }

    function supportsInterface(bytes4 interfaceId) public pure returns (bool)
    { return interfaceId == 0x80ac58cd; }

    function transferFromImpl(EventData storage self,address _from, address payable _to, uint24 _token) public {
      require(self.d_token_owner[_token] == _from);
      
      // Value provided, okay to transfer
      if (self.d_token_ask[_token] != 0) {
        delete self.d_token_ask[_token]; // No more ask 
        self.d_token_ask_num--;
      }
        
      address prev_owner = self.d_token_owner[_token];
        
      uint24[] storage prev_owner_tokens = self.d_owner_tokens[prev_owner];
      for (uint24 i = 0;i<prev_owner_tokens.length; ++i) {
        if (prev_owner_tokens[i] == _token) {
            prev_owner_tokens[i] = prev_owner_tokens[prev_owner_tokens.length-1];
            prev_owner_tokens.pop();
            break;
        }
      }
        
      self.d_token_owner[_token] = _to;
      self.d_owner_tokens[_to].push(_token);
  }

}
