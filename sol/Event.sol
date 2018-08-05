pragma solidity ^0.4.24;

import "./ERC721.sol";

contract Event is ERC721 {
    
  struct TicketInfo {
    uint256 d_orig_price;
  }
 
  uint256 internal d_creator_commission_factor;
 
  address internal d_admin;
  address internal d_organizer;
  
  string internal d_description;
  
  // Array with all token ids, used for enumeration
  TicketInfo[] internal d_tickets;
  
  // Mapping from owner to list of owned token IDs
  mapping(address => uint256[]) internal d_owner_tokens;
  
  // Mapping from token id to owner address
  mapping(uint256 => address) internal d_token_owner;
  

  constructor(string _description, address _organizer) public { 
    d_admin = msg.sender;
    d_organizer = _organizer;
    d_description = _description;
  }
  
  function issue(uint256 _numTickets,uint256 _price) public {
    for(uint256 i=0;i<_numTickets;++i) {
      d_tickets.push(TicketInfo({d_orig_price:_price}));
    }
  }
  
  function buy(uint256 _numTickets) public payable {
    uint256 total_cost=0;
    uint256 bought=0;
    
    // We will buy 1 ticket at a time
    // If while buying, we do not find enough tickets, 
    // or we did not get enough money, we throw
    for(uint256 i=0;i<d_tickets.length && i < _numTickets;++i) {
      if (d_token_owner[i] != address(0)) { continue; }
        
      // Ticket can be bought 
      total_cost+=d_tickets[i].d_orig_price;
      d_owner_tokens[msg.sender].push(i);
      d_token_owner[i] = msg.sender;
      bought++;
  
    }
    
    require(bought == _numTickets);
    require(total_cost <= msg.value);
    
    // Take admin cut
    address(d_admin).transfer(msg.value / d_creator_commission_factor);
  }
  
  function getBalance() public view returns(uint) {
    require(msg.sender == d_admin || msg.sender == d_organizer);
    return address(this).balance;
  }
  
  function withdraw() public {
    require(msg.sender == d_organizer);
    address(d_organizer).transfer(getBalance());
  }
  
  function numSold() public view returns(uint256) {
    require(msg.sender == d_admin || msg.sender == d_organizer);
    uint256 numSoldCount=0;
    for(uint256 i=0;i<d_tickets.length;++i) {
      if (d_token_owner[i] != address(0)) { numSoldCount++;}
    }
    return numSoldCount;
  }
  
  function numUnSold() public view returns(uint256) {
    require(msg.sender == d_admin || msg.sender == d_organizer);
    uint256 numUnSoldCount=0;
    for(uint256 i=0;i<d_tickets.length;++i) {
      if (d_token_owner[i] == address(0)) { numUnSoldCount++; }
    }
    return numUnSoldCount;
  }
  
  function description() public view returns(string) {
    return d_description;
  }
  
  function balanceOf(address _owner) public view returns (uint256 _balance) {
    return d_owner_tokens[_owner].length;    
  }
  
  function ownerOf(uint256 _tokenId) public view returns (address _owner) {
    return d_token_owner[_tokenId];    
  }
  
  function exists(uint256 _tokenId) public view returns (bool _exists) {
    return _tokenId >= 0 && _tokenId < d_tickets.length;
  }

  function approve(address _to, uint256 _tokenId) public;
  function getApproved(uint256 _tokenId)
    public view returns (address _operator);

  function setApprovalForAll(address _operator, bool _approved) public;
  function isApprovedForAll(address _owner, address _operator)
    public view returns (bool);

  function transferFrom(address _from, address _to, uint256 _tokenId) public;
  function safeTransferFrom(address _from, address _to, uint256 _tokenId) public;

  function safeTransferFrom(
    address _from,
    address _to,
    uint256 _tokenId,
    bytes _data
  ) public;

  
  
  
}