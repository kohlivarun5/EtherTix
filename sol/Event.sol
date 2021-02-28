// SPDX-License-Identifier: UNLICENSED
pragma solidity >=0.6.0 <0.8.0;

import "./IERC721.sol";

import "./Universe.sol";
import "./EventImpl.sol";

contract Event is IERC721 {
    
  string public description;
  string public imgSrc; // Can be data or link
  string public externalLink;
  bool public d_mark_delete;

  using EventImpl for EventData;
  EventData internal d_data;

  constructor(string memory _description, address payable _organizer,uint256 commission_percent) public { 
    description = _description;
    d_data.d_admin = msg.sender;
    d_data.d_organizer=_organizer;
    d_data.d_token_ask_num=0;
    d_data.d_creator_commission_percent=commission_percent;
    d_mark_delete=false;
  }
  
  function setImg(string memory _imgSrc) public {
      require(msg.sender == d_data.d_organizer);
      imgSrc = _imgSrc;
  }
  
  function setExternalLink(string memory _externalLink) public {
      require(msg.sender == d_data.d_organizer);
      externalLink = _externalLink;
  }
  
  function issue(uint256 _numTickets,uint256 _price) public {
    require(msg.sender == d_data.d_organizer);
    for(uint256 i=0;i<_numTickets;++i) {
      d_data.d_tickets.push(TicketInfo({d_prev_price:_price,d_used:false}));
    }
  }

  function getCostFor(uint256 _numTickets) public view returns(uint256) {
    return d_data.getCostFor(_numTickets);
  }
  
  function buy(uint256 _numTickets) public payable {
    return d_data.buy(_numTickets,msg.sender);
  }
  
  function getBalance() public view returns(uint) {
    require(msg.sender == d_data.d_admin || msg.sender == d_data.d_organizer);
    return address(this).balance;
  }
  
  function withdraw() public {
    require(msg.sender == d_data.d_organizer);
    return d_data.d_organizer.transfer(getBalance());
  }
  
  function numSold() public view returns(uint256) {
    return d_data.numSold();
  }
  
  function numUnSold() public view returns(uint256) {
    return d_data.numUnSold();
  }

  function numUsed() public view returns(uint256) {
    return d_data.numUsed();
  }

  function numToBeUsed() public view returns(uint256) {
    return d_data.numToBeUsed();
  }
  
  function balanceOf(address _owner) public view override returns (uint256) {
    return d_data.d_owner_tokens[_owner].length;    
  }
  
  function ownerOf(uint256 _tokenId) public view override returns (address) {
    return d_data.d_token_owner[_tokenId];    
  }
  
  function exists(uint256 _tokenId) public view returns (bool _exists) {
    return _tokenId >= 0 && _tokenId < d_data.d_tickets.length;
  }
  
  function myTickets() public view returns(uint256[] memory,uint256[] memory,bool[] memory,bool[] memory) {
    return d_data.myTickets(msg.sender);
  }
  
  function proposeSale(uint256 _token,uint256 _price) public {
    require(d_data.d_tickets[_token].d_used == false, "Ticket already used!");
    require(d_data.d_token_owner[_token] == msg.sender);
    require(_price > 0, "Please set a valid non-zero price");
    if (d_data.d_token_ask[_token] == 0) {
        d_data.d_token_ask_num++;
    }
    d_data.d_token_ask[_token] = _price;
  }
  
  function proposeSales(uint256[] memory _tokens,uint256[] memory _prices) public {
      require(_tokens.length == _prices.length);
      for(uint256 i=0;i<_tokens.length;++i) {
        proposeSale(_tokens[i],_prices[i]);
      }
  }
  
  function retractSale(uint256 _token) public {
    require(d_data.d_token_owner[_token] == msg.sender);
    require(d_data.d_token_ask[_token] != 0);
    delete d_data.d_token_ask[_token];
    d_data.d_token_ask_num--;
  }
  
  function retractSales(uint256[] memory _tokens) public {
    for(uint256 i=0;i<_tokens.length;++i) {
      retractSale(_tokens[i]);
    }
  }

  function forSale() public view returns(uint256,uint256[] memory,uint256[] memory) {
    return d_data.forSale();
  }
  
  function hitAsk(uint256 _token) public payable {
    require(!d_data.d_tickets[_token].d_used, "Ticket already used!");
    require(d_data.d_token_ask[_token] > 0 && msg.value >= d_data.d_token_ask[_token]);
      
    // Value provided, okay to transfer
    address payable prev_owner = d_data.d_token_owner[_token];
    transferFromImpl(prev_owner,msg.sender,_token);
    
    // Take money
    if (d_data.d_tickets[_token].d_prev_price > msg.value) {
      // Selling for less, all money to seller 
      prev_owner.transfer(msg.value);
    } else {
      uint256 premium = msg.value - d_data.d_tickets[_token].d_prev_price;
      uint256 seller_premium = premium / 2;
      
      // TODO Review
      prev_owner.transfer(seller_premium + d_data.d_tickets[_token].d_prev_price);
      
      // Other half premium is for the event, and commission out of it 
      uint256 commission = (seller_premium / 100) * d_data.d_creator_commission_percent;
      d_data.d_admin.transfer(commission);
      
      d_data.d_tickets[_token].d_prev_price = msg.value;
    }
  }
  
  function ticketUsed(uint256 _tokenId) public view returns(bool) {
      return d_data.d_tickets[_tokenId].d_used;
  }

  // https://medium.com/@libertylocked/ec-signatures-and-recovery-in-ethereum-smart-contracts-560b6dd8876
  function ticketVerificationCode(uint256 _tokenId) public view returns(bytes32) {
    return d_data.ticketVerificationCode(_tokenId);
  }
  
  function isOwnerSig(uint256 _tokenId, bytes memory signature) public view returns(bool) {
    return d_data.isOwnerSig(_tokenId, signature);
  }

  function useTicket(uint256 _tokenId) public returns(bool) {
    require(msg.sender == d_data.d_organizer);
    require(!ticketUsed(_tokenId), "Ticket already used!");
    d_data.d_tickets[_tokenId].d_used = true;
    if (d_data.d_token_ask[_tokenId] != 0) {
        delete d_data.d_token_ask[_tokenId]; // Just in case
        d_data.d_token_ask_num--;
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

  function transferFromImpl(address _from, address payable _to, uint256 _token) private {
      require(d_data.d_token_owner[_token] == _from);
      
      // Value provided, okay to transfer
      if (d_data.d_token_ask[_token] != 0) {
        delete d_data.d_token_ask[_token]; // No more ask 
        d_data.d_token_ask_num--;
      }
        
      address prev_owner = d_data.d_token_owner[_token];
        
      uint256[] storage prev_owner_tokens = d_data.d_owner_tokens[prev_owner];
      for (uint256 i = 0;i<prev_owner_tokens.length; ++i) {
        if (prev_owner_tokens[i] == _token) {
            uint256 lenBefore = prev_owner_tokens.length;
            prev_owner_tokens[i] = prev_owner_tokens[lenBefore-1];
            prev_owner_tokens.pop();
            break;
        }
      }
        
      d_data.d_token_owner[_token] = _to;
      d_data.d_owner_tokens[_to].push(_token);
      emit Transfer(_from,_to,_token);
        
      Universe u = Universe(d_data.d_admin);
      u.addUserEvent(address(this),_to);
  }

  function transferFrom(address _from, address payable _to, uint256 _token) public override {
      require(d_data.d_token_owner[_token] == _from);
      require(msg.sender == _from || tx.origin == _from);
      transferFromImpl(_from,_to,_token);
  }

  function safeTransferFrom(address _from, address payable _to, uint256 _tokenId) public override
  { return transferFrom(_from,_to,_tokenId);}

  function markDelete(bool mark_delete) public {
    require(msg.sender == d_data.d_organizer);
    d_mark_delete=mark_delete;
  }
  function supportsInterface(bytes4 interfaceId) external view override returns (bool)
  { return EventImpl.supportsInterface(interfaceId); }

}


