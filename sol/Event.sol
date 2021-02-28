pragma solidity >=0.6.0 <0.8.0;

import "./IERC721.sol";

import "./Universe.sol";

contract Event is IERC721 {
    
  struct TicketInfo {
    uint256 d_prev_price;
    bool    d_used;
  }
 
  string public description;
  string public imgSrc; // Can be data or link
  string public externalLink;
 
  uint256 internal d_creator_commission_percent = 1;
 
  address payable internal d_admin;
  address payable public d_organizer;
  
  
  // Array with all token ids, used for enumeration
  TicketInfo[] internal d_tickets;
  
  // Mapping from owner to list of owned token IDs
  mapping(address => uint256[]) internal d_owner_tokens;
  
  // Mapping from token id to owner address
  mapping(uint256 => address payable) internal d_token_owner;
  
  // For transfers 
  mapping(uint256 => uint256) internal d_token_ask;
  uint256 d_token_ask_num;

  bool public d_mark_delete;

  constructor(string memory _description, address payable _organizer,uint256 commission_percent) public { 
    description = _description;
    d_admin = msg.sender;
    d_organizer = _organizer;
    d_token_ask_num=0;
    d_creator_commission_percent=commission_percent;
    d_mark_delete=false;
  }
  
  function setImg(string memory _imgSrc) public {
      require(msg.sender == d_organizer);
      imgSrc = _imgSrc;
  }
  
  function setExternalLink(string memory _externalLink) public {
      require(msg.sender == d_organizer);
      externalLink = _externalLink;
  }
  
  function issue(uint256 _numTickets,uint256 _price) public {
    require(msg.sender == d_organizer);
    for(uint256 i=0;i<_numTickets;++i) {
      d_tickets.push(TicketInfo({d_prev_price:_price,d_used:false}));
    }
  }

  function getCostFor(uint256 _numTickets) public view returns(uint256) {
    uint256 total_cost=0;
    uint256 bought=0;
    
    // We will buy 1 ticket at a time
    // If while buying, we do not find enough tickets, 
    // or we did not get enough money, we throw
    for(uint256 i=0;i<d_tickets.length && bought < _numTickets;++i) {
      if (d_token_owner[i] != address(0)) { continue; }
        
      // Ticket can be bought 
      total_cost+=d_tickets[i].d_prev_price;
      bought++;
    }
    
    require(bought == _numTickets, "Not enough tickets!");

    return total_cost;
  }
  
  function buy(uint256 _numTickets) public payable {
    uint256 total_cost=0;
    uint256 bought=0;
    
    // We will buy 1 ticket at a time
    // If while buying, we do not find enough tickets, 
    // or we did not get enough money, we throw
    for(uint256 i=0;i<d_tickets.length && bought < _numTickets;++i) {
      if (d_token_owner[i] != address(0)) { continue; }
        
      // Ticket can be bought 
      total_cost+=d_tickets[i].d_prev_price;
      d_owner_tokens[msg.sender].push(i);
      d_token_owner[i] = msg.sender;
      bought++;
  
    }
    
    require(bought == _numTickets, "Not enough tickets!");
    require(total_cost <= msg.value, "Cost is more than transaction value.");
    
    // Take admin cut
    uint256 commission = (msg.value / 100) * d_creator_commission_percent;
    d_admin.transfer(commission);
    
    Universe u = Universe(d_admin);
    u.addUserEvent(address(this),msg.sender);
  }
  
  function getBalance() public view returns(uint) {
    require(msg.sender == d_admin || msg.sender == d_organizer);
    return address(this).balance;
  }
  
  function withdraw() public {
    require(msg.sender == d_organizer);
    d_organizer.transfer(getBalance());
  }
  
  function numSold() public view returns(uint256) {
    uint256 numSoldCount=0;
    for(uint256 i=0;i<d_tickets.length;++i) {
      if (d_token_owner[i] != address(0)) { numSoldCount++;}
    }
    return numSoldCount;
  }
  
  function numUnSold() public view returns(uint256) {
    uint256 numUnSoldCount=0;
    for(uint256 i=0;i<d_tickets.length;++i) {
      if (d_token_owner[i] == address(0)) { numUnSoldCount++; }
    }
    return numUnSoldCount;
  }

  function numUsed() public view returns(uint256) {
    uint256 numUsedCount=0;
    for(uint256 i=0;i<d_tickets.length;++i) {
      if (d_tickets[i].d_used) { numUsedCount++; } 
    }
    return numUsedCount;
  }

  function numToBeUsed() public view returns(uint256) {
    uint256 numToBeUsedCount=0;
    for(uint256 i=0;i<d_tickets.length;++i) {
      if (!d_tickets[i].d_used && d_token_owner[i] != address(0)) { numToBeUsedCount++; } 
    }
    return numToBeUsedCount;
  }
  
  function balanceOf(address _owner) public view override returns (uint256 _balance) {
    return d_owner_tokens[_owner].length;    
  }
  
  function ownerOf(uint256 _tokenId) public view override returns (address _owner) {
    return d_token_owner[_tokenId];    
  }
  
  function exists(uint256 _tokenId) public view returns (bool _exists) {
    return _tokenId >= 0 && _tokenId < d_tickets.length;
  }
  
  function myTickets() public view returns(uint256[] memory,uint256[] memory,bool[] memory,bool[] memory) {
    uint256[] storage tokens = d_owner_tokens[msg.sender];
    uint256[] memory prices = new uint256[](tokens.length);
    bool[] memory for_sale = new bool[](tokens.length);
    bool[] memory used = new bool[](tokens.length);
    for(uint256 i=0;i<tokens.length;++i)
    {
        uint256 token=tokens[i];
        if (d_token_ask[token] != 0) {
            prices[i] = d_token_ask[token];
            for_sale[i] = true;
        } else {
            prices[i] = d_tickets[token].d_prev_price;
            for_sale[i] = false;
        }
        used[i] = d_tickets[token].d_used;
    }
    return (tokens,prices,for_sale,used);
  }
  
  function proposeSale(uint256 _token,uint256 _price) public {
    require(d_tickets[_token].d_used == false, "Ticket already used!");
    require(d_token_owner[_token] == msg.sender);
    require(_price > 0, "Please set a valid non-zero price");
    if (d_token_ask[_token] == 0) {
        d_token_ask_num++;
    }
    d_token_ask[_token] = _price;
  }
  
  function proposeSales(uint256[] memory _tokens,uint256[] memory _prices) public {
      require(_tokens.length == _prices.length);
      for(uint256 i=0;i<_tokens.length;++i) {
        proposeSale(_tokens[i],_prices[i]);
      }
  }
  
  function retractSale(uint256 _token) public {
    require(d_token_owner[_token] == msg.sender);
    require(d_token_ask[_token] != 0);
    delete d_token_ask[_token];
    d_token_ask_num--;
  }
  
  function retractSales(uint256[] memory _tokens) public {
    for(uint256 i=0;i<_tokens.length;++i) {
      retractSale(_tokens[i]);
    }
  }

  function forSale() public view returns(uint256,uint256[] memory,uint256[] memory) {
    uint256[] memory tokens = new uint256[](d_token_ask_num);
    uint256[] memory asks = new uint256[](d_token_ask_num);
    uint256 iter=0;
    for(uint256 i=0;i<d_tickets.length;++i)
    {
        if (d_token_ask[i] > 0) { 
            tokens[iter] = i;
            asks[iter] = d_token_ask[i];
            iter++;
        }
    }
    require(iter == d_token_ask_num);
    return (iter,tokens,asks);
  }
  
  function hitAsk(uint256 _token) public payable {
    require(!d_tickets[_token].d_used, "Ticket already used!");
    require(d_token_ask[_token] > 0 && msg.value >= d_token_ask[_token]);
      
    // Value provided, okay to transfer
    address payable prev_owner = d_token_owner[_token];
    transferFromImpl(prev_owner,msg.sender,_token);
    
    // Take money
    if (d_tickets[_token].d_prev_price > msg.value) {
      // Selling for less, all money to seller 
      prev_owner.transfer(msg.value);
    } else {
      uint256 premium = msg.value - d_tickets[_token].d_prev_price;
      uint256 seller_premium = premium / 2;
      
      // TODO Review
      prev_owner.transfer(seller_premium + d_tickets[_token].d_prev_price);
      
      // Other half premium is for the event, and commission out of it 
      uint256 commission = (seller_premium / 100) * d_creator_commission_percent;
      d_admin.transfer(commission);
      
      d_tickets[_token].d_prev_price = msg.value;
    }
  }
  
  function ticketUsed(uint256 _tokenId) public view returns(bool) {
      return d_tickets[_tokenId].d_used;
  }

  // https://medium.com/@libertylocked/ec-signatures-and-recovery-in-ethereum-smart-contracts-560b6dd8876
  function ticketVerificationCode(uint256 _tokenId) public view returns(bytes32) {
    require(!ticketUsed(_tokenId), "Ticket already used!");
    return keccak256(abi.encodePacked(_tokenId,address(this)));
  }
  
  function isOwnerSig(uint256 _tokenId, bytes memory signature) public view returns(bool) {
    bytes memory prefix = "\x19Ethereum Signed Message:\n32";
    bytes32 prefixedHash = keccak256(abi.encodePacked(prefix, ticketVerificationCode(_tokenId)));
    return d_token_owner[_tokenId] == 
            recover(prefixedHash,signature);
  }

  function useTicket(uint256 _tokenId) public returns(bool) {
    require(msg.sender == d_organizer);
    require(!ticketUsed(_tokenId), "Ticket already used!");
    d_tickets[_tokenId].d_used = true;
    if (d_token_ask[_tokenId] != 0) {
        delete d_token_ask[_tokenId]; // Just in case
        d_token_ask_num--;
    }
    return true;
  }

  function useTicket(uint256 _tokenId, bytes memory signature) public returns(bool) {
    require(isOwnerSig(_tokenId,signature), "Incorrect usage signature!");
    return useTicket(_tokenId);
  }

/* TODO Cannot use since we cannot pass in bytes[] as an argument
  function useTickets(uint256[10] _tokenIds, bytes signatures) public returns(bool) {
    require(_tokenIds.length == signatures.length);
    for(uint256 i=0;i<_tokenIds.length && i < signatures.length; ++i) {
      useTicket(_tokenIds[i],signatures[i]);
    }
    return true;
  }
*/

  function recover(bytes32 message, bytes memory signature) internal pure returns (address) {
      (uint8 v, bytes32 r, bytes32 s) = splitSignature(signature);
      return ecrecover(message,v, r, s);
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
  
  function transferFromImpl(address _from, address payable _to, uint256 _token) private {
      require(d_token_owner[_token] == _from);
      
      // Value provided, okay to transfer
      if (d_token_ask[_token] != 0) {
        delete d_token_ask[_token]; // No more ask 
        d_token_ask_num--;
      }
        
      address prev_owner = d_token_owner[_token];
        
      uint256[] storage prev_owner_tokens = d_owner_tokens[prev_owner];
      for (uint256 i = 0;i<prev_owner_tokens.length; ++i) {
        if (prev_owner_tokens[i] == _token) {
            uint256 lenBefore = prev_owner_tokens.length;
            prev_owner_tokens[i] = prev_owner_tokens[lenBefore-1];
            prev_owner_tokens.pop();
            break;
        }
      }
        
      d_token_owner[_token] = _to;
      d_owner_tokens[_to].push(_token);
        
      Universe u = Universe(d_admin);
      u.addUserEvent(address(this),_to);
  }

  function transferFrom(address _from, address _to, uint256 _token) public override {
      require(d_token_owner[_token] == _from);
      require(msg.sender == _from || tx.origin == _from);
      transferFromImpl(_from,payable(_to),_token);
  }

  function markDelete(bool mark_delete) public {
    require(msg.sender == d_organizer);
    d_mark_delete=mark_delete;
  }

  function approve(address _to, uint256 _tokenId) public override 
  { require(false,"Unsupported"); }
  function getApproved(uint256 _tokenId) public view override returns (address _operator) 
  { require(false,"Unsupported"); }

  function setApprovalForAll(address _operator, bool _approved) public override
  { require(false,"Unsupported"); }
  function isApprovedForAll(address _owner, address _operator) public view override returns (bool) 
  { require(false,"Unsupported"); }

  function safeTransferFrom(address _from, address _to, uint256 _tokenId) public override
  { require(false,"Unsupported"); }

  function safeTransferFrom(address from, address to, uint256 tokenId, bytes calldata data) public override
  { require(false,"Unsupported"); }
  function supportsInterface(bytes4 interfaceId) external view override returns (bool)
  { require(false,"Unsupported"); }

}


