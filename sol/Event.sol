pragma solidity ^0.4.24;

import "./ERC721.sol";

contract Event /* is ERC721 */  {
    
  struct TicketInfo {
    uint256 d_orig_price; // Prices are denominated in szabo
  }
 
  uint256 internal d_creator_commission_factor = 100; /* 1% commission */
 
  address internal d_admin;
  address internal d_organizer;
  
  string internal d_description;
  
  // Array with all token ids, used for enumeration
  TicketInfo[] internal d_tickets;
  
  // Mapping from owner to list of owned token IDs
  mapping(address => uint256[]) internal d_owner_tokens;
  
  // Mapping from token id to owner address
  mapping(uint256 => address) internal d_token_owner;
  
  // For transfers 
  mapping(uint256 => uint256) internal d_token_ask;
  

  constructor(string _description, address _organizer) public { 
    d_admin = msg.sender;
    d_organizer = _organizer;
    d_description = _description;
  }
  
  function issue(uint256 _numTickets,uint256 _price_szabo) public {
    require(msg.sender == d_organizer);
    // require(_price > d_creator_commission_factor * 1 szabo,
    //         "Minimum cost is 100 szabo"); // Denominate in szabo
    
    for(uint256 i=0;i<_numTickets;++i) {
      d_tickets.push(TicketInfo({d_orig_price:(_price_szabo * 1 szabo)}));
    }
  }
  
  function buy(uint256 _numTickets) public payable returns(uint256) {
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
    
    require(bought == _numTickets, "Not enough tickets!");
    require(total_cost <= msg.value, "Cost is more than transaction value.");
    
    // Take admin cut
    uint256 commission = msg.value / d_creator_commission_factor;
    address(d_admin).transfer(commission);
    return commission;
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
  
  function myTickets() public view returns(uint256[]) {
    return d_owner_tokens[msg.sender];
  }
  
  function proposeSale(uint256 _token,uint256 _price) public {
    require(d_token_owner[_token] == msg.sender);
    d_token_ask[_token] = _price;
  }
  
  function retractSale(uint256 _token) public {
    require(d_token_owner[_token] == msg.sender);
    delete d_token_ask[_token];
  }
  
  function hitAsk(uint256 _token) public payable {
    require(d_token_ask[_token] > 0 && msg.value > d_token_ask[_token]);
      
    // Value provided, okay to transfer
    delete d_token_ask[_token]; // No more ask 
    
    address prev_owner = d_token_owner[_token];
    d_token_owner[_token] = msg.sender;
    d_owner_tokens[msg.sender].push(_token);
      
    for (uint256 i = 0;i<d_owner_tokens[prev_owner].length; ++i) {
      if (d_owner_tokens[prev_owner][i] == _token) {
        d_owner_tokens[prev_owner][i] = d_owner_tokens[prev_owner][d_owner_tokens[prev_owner].length-1];
        delete d_owner_tokens[prev_owner][d_owner_tokens[prev_owner].length-1];
      }
    }
    
    // Take money
    if (d_tickets[_token].d_orig_price > msg.value) {
      // Selling for less, all money to seller 
      address(prev_owner).transfer(msg.value);
    } else {
      uint256 premium = msg.value - d_tickets[_token].d_orig_price;
      uint256 seller_premium = premium / 2;
      
      address(prev_owner).transfer(seller_premium + d_tickets[_token].d_orig_price);
      
      // Other half premium is for the event, and commission out of it 
      uint256 commission = seller_premium / d_creator_commission_factor;
      address(d_admin).transfer(commission);
    }
  }

/*
  function approve(address _to, uint256 _tokenId) public;
  function getApproved(uint256 _tokenId) public view returns (address _operator);

  function setApprovalForAll(address _operator, bool _approved) public;
  function isApprovedForAll(address _owner, address _operator) public view returns (bool);

  function transferFrom(address _from, address _to, uint256 _tokenId) public;
  function safeTransferFrom(address _from, address _to, uint256 _tokenId) public;
*/
}