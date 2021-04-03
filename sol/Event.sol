// SPDX-License-Identifier: UNLICENSED
pragma solidity >=0.6.0 <0.8.0;

import "./IERC721.sol";
import "./IERC721Metadata.sol";
import "./IERC721Enumerable.sol";

import "./Universe.sol";
import "./EventImpl.sol";

contract Event is ERC721,ERC721Metadata,ERC721Enumerable {
    
  using EventImpl for EventData;
  EventData internal d_data;
  
  string private d_description;
  string public imgSrc; // Can be data or link
  string public externalLink;

  constructor(address payable _organizer,uint8 commission_percent,string memory _description,string memory _imgSrc) public { 
    d_description = _description;
    imgSrc = _imgSrc;
    d_data.d_admin = msg.sender;
    d_data.d_organizer=_organizer;
    d_data.d_creator_commission_percent=commission_percent;
  }
  
  function setImg(string memory _imgSrc) public {
      require(msg.sender == d_data.d_organizer);
      imgSrc = _imgSrc;
  }
  
  function setExternalLink(string memory _externalLink) public {
      require(msg.sender == d_data.d_organizer);
      externalLink = _externalLink;
  }
  
  function issue(uint24 _numTickets,uint _price) public {
    require(msg.sender == d_data.d_organizer);
    for(uint24 i=0;i<_numTickets;++i) {
      d_data.d_tickets.push(TicketInfo({d_prev_price:_price,d_used:false}));
    }
  }

  function getCostFor(uint24 _numTickets) public view returns(uint) {
    return d_data.getCostFor(_numTickets);
  }
  
  function buy(uint24 _numTickets) public payable {
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
  
  function numSold() public view returns(uint24) {
    return d_data.numSold();
  }
  
  function numUnSold() public view returns(uint24) {
    return d_data.numUnSold();
  }

  function numUsed() public view returns(uint24) {
    return d_data.numUsed();
  }

  function numToBeUsed() public view returns(uint24) {
    return d_data.numToBeUsed();
  }
  
  function balanceOf(address _owner) public view override returns (uint256) {
    return d_data.d_owner_tokens[_owner].length;    
  }
  
  function ownerOf(uint24 _tokenId) public view override returns (address) {
    return d_data.d_token_owner[_tokenId];    
  }
  
  function exists(uint24 _tokenId) public view returns (bool _exists) {
    return _tokenId >= 0 && _tokenId < d_data.d_tickets.length;
  }
  
  function myTickets() public view returns(uint24[] memory,uint[] memory,bool[] memory,bool[] memory) {
    return d_data.myTickets(msg.sender);
  }
  
  function proposeSale(uint24 _token,uint _price) public {
    require(d_data.d_tickets[_token].d_used == false, "Ticket already used!");
    require(d_data.d_token_owner[_token] == msg.sender);
    require(_price > 0, "Please set a valid non-zero price");
    if (d_data.d_token_ask[_token] == 0) {
        d_data.d_token_ask_num++;
    }
    d_data.d_token_ask[_token] = _price;
  }
  
  function proposeSales(uint24[] memory _tokens,uint[] memory _prices) public {
      require(_tokens.length == _prices.length);
      for(uint24 i=0;i<_tokens.length;++i) {
        proposeSale(_tokens[i],_prices[i]);
      }
  }
  
  function retractSale(uint24 _token) public {
    require(d_data.d_token_owner[_token] == msg.sender);
    require(d_data.d_token_ask[_token] != 0);
    delete d_data.d_token_ask[_token];
    d_data.d_token_ask_num--;
  }
  
  function retractSales(uint24[] memory _tokens) public {
    for(uint24 i=0;i<_tokens.length;++i) {
      retractSale(_tokens[i]);
    }
  }

  function forSale() public view returns(uint24,uint24[] memory,uint[] memory) {
    return d_data.forSale();
  }
  
  function hitAsk(uint24 _token) public payable {
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
      uint premium = msg.value - d_data.d_tickets[_token].d_prev_price;
      uint seller_premium = premium / 2;
      
      // TODO Review
      prev_owner.transfer(seller_premium + d_data.d_tickets[_token].d_prev_price);
      
      // Other half premium is for the event, and commission out of it 
      uint commission = (seller_premium / 100) * d_data.d_creator_commission_percent;
      d_data.d_admin.transfer(commission);
      
      d_data.d_tickets[_token].d_prev_price = msg.value;
    }
  }
  
  function ticketUsed(uint24 _tokenId) public view returns(bool) {
      return d_data.d_tickets[_tokenId].d_used;
  }

  // https://medium.com/@libertylocked/ec-signatures-and-recovery-in-ethereum-smart-contracts-560b6dd8876
  function ticketVerificationCode(uint24 _tokenId) public view returns(bytes32) {
    return d_data.ticketVerificationCode(_tokenId);
  }
  
  function isOwnerSig(uint24 _tokenId, bytes memory signature) public view returns(bool) {
    return d_data.isOwnerSig(_tokenId, signature);
  }

  function useTicket(uint24 _tokenId) public returns(bool) {
    require(msg.sender == d_data.d_organizer);
    require(!ticketUsed(_tokenId), "Ticket already used!");
    d_data.d_tickets[_tokenId].d_used = true;
    if (d_data.d_token_ask[_tokenId] != 0) {
        delete d_data.d_token_ask[_tokenId]; // Just in case
        d_data.d_token_ask_num--;
    }
    return true;
  }

  function useTicket(uint24 _tokenId, bytes memory signature) public returns(bool) {
    require(isOwnerSig(_tokenId,signature), "Incorrect usage signature!");
    return useTicket(_tokenId);
  }

  function transferFromImpl(address _from, address payable _to, uint24 _token) private {
      require(d_data.d_token_owner[_token] == _from);
      
      // Value provided, okay to transfer
      if (d_data.d_token_ask[_token] != 0) {
        delete d_data.d_token_ask[_token]; // No more ask 
        d_data.d_token_ask_num--;
      }
        
      address prev_owner = d_data.d_token_owner[_token];
        
      uint24[] storage prev_owner_tokens = d_data.d_owner_tokens[prev_owner];
      for (uint24 i = 0;i<prev_owner_tokens.length; ++i) {
        if (prev_owner_tokens[i] == _token) {
            prev_owner_tokens[i] = prev_owner_tokens[prev_owner_tokens.length-1];
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

  function transferFrom(address _from, address payable _to, uint24 _token) public override {
      require(d_data.d_token_owner[_token] == _from);
      require(msg.sender == _from || tx.origin == _from);
      transferFromImpl(_from,_to,_token);
  }

  function safeTransferFrom(address _from, address payable _to, uint24 _tokenId) public override
  { return transferFrom(_from,_to,_tokenId);}

  // function markDelete(bool mark_delete) public {
  //   require(msg.sender == d_data.d_organizer);
  //   d_data.d_mark_delete=mark_delete;
  // }

  function supportsInterface(bytes4 interfaceId) public pure override returns (bool)
  { return EventImpl.supportsInterface(interfaceId); }

  // ERC721MetaData
  function name() external view override returns (string memory _name)
  { return d_description; }

  function symbol() external pure override returns (string memory _symbol)
  { return "ETIX"; }

  function tokenURI(uint24) external view override returns (string memory)
  { return imgSrc; }

  // ERC721Enumerable
  function totalSupply() public view override returns (uint256)
  { return d_data.d_tickets.length; }

  function tokenByIndex(uint24 _index) external view override returns (uint24)
  { return _index; }

  function tokenOfOwnerByIndex(address _owner, uint24 _index) external view override returns (uint24 _tokenId)
  {
    require(_index < d_data.d_owner_tokens[_owner].length);
    return d_data.d_owner_tokens[_owner][_index];
  }


}