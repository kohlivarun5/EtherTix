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
  
  event EventAsk(address indexed updator,uint24 indexed token,uint indexed ask);

  string private d_description;
  string public imgSrc;

  constructor(address payable _organizer,uint8 commission_percent,string memory description,string memory _imgSrc) public { 
    d_description = description;
    imgSrc = _imgSrc;
    d_data.d_admin = msg.sender;
    d_data.d_organizer=_organizer;
    d_data.d_creator_commission_percent=commission_percent;
  }

  // ERC 721
  function balanceOf(address _owner) public view override returns (uint) {
    return d_data.d_owner_tokens[_owner].length;    
  }
  
  function ownerOf(uint24 _tokenId) public view override returns (address) {
    return d_data.d_token_owner[_tokenId];    
  }

  function transferFrom(address _from, address payable _to, uint24 _token) public override {
      require(d_data.d_token_owner[_token] == _from);
      require(msg.sender == _from || tx.origin == _from);
      transferFromImpl(_from,_to,_token);
  }

  function safeTransferFrom(address _from, address payable _to, uint24 _tokenId) public override
  { return transferFrom(_from,_to,_tokenId);}

  function supportsInterface(bytes4 interfaceId) public pure override returns (bool)
  { return EventImpl.supportsInterface(interfaceId); }

  // ERC721MetaData
  function name() external view override returns (string memory)
  { return d_description; }

  function symbol() external pure override returns (string memory)
  { return "ETIX"; }

  // function tokenURI(uint24) external view override returns (string memory)
  // { return imgSrc; }

  // ERC721Enumerable
  function totalSupply() public view override returns (uint256)
  { return d_data.d_tickets.length; }

  function tokenByIndex(uint24 index) external view override returns (uint24)
  { return index; }

  function tokenOfOwnerByIndex(address owner, uint24 index) external view override returns (uint24)
  {
    require(index >=0 && index < d_data.d_owner_tokens[owner].length);
    return d_data.d_owner_tokens[owner][index];
  }
  
  function updateImg(string memory _imgSrc) public {
    imgSrc = _imgSrc;
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

  function withdraw() public {
    require(msg.sender == d_data.d_organizer);
    return d_data.d_organizer.transfer(address(this).balance);
  }

  function numSoldUsed() public view returns(uint24 ,uint24 ) {
    return d_data.numSoldUsed();
  }
  
  function ticketInfo(uint24 index) public view returns(bool used, uint prev_price, address owner, bool forSale,uint ask) {
    return d_data.ticketInfo(index);
  }
  
  function proposeSales(uint24[] memory tokens,uint[] memory prices) public {
      require(tokens.length == prices.length);
      for(uint24 i=0;i<tokens.length;++i) {
        require(d_data.d_token_owner[tokens[i]] == msg.sender);
        d_data.proposeSaleUnsafe(tokens[i],prices[i]);
        emit EventAsk(msg.sender,tokens[i],prices[i]);
      }
  }

  function retractSales(uint24[] memory tokens) public {
    for(uint24 i=0;i<tokens.length;++i) {
      require(d_data.d_token_owner[tokens[i]] == msg.sender);
      d_data.retractSaleUnsafe(tokens[i]);
    }
  }

  function hitAsk(uint24 _token) public payable {
    require(!d_data.d_tickets[_token].d_used, "Ticket already used!");
    require(d_data.d_token_ask[_token] > 0 && msg.value >= d_data.d_token_ask[_token]);
      
    // Value provided, okay to transfer
    address payable prev_owner = d_data.d_token_owner[_token];
    transferFromImpl(prev_owner,msg.sender,_token);
    return d_data.hitAskPostTransfer(_token, prev_owner);
  }
  
  // https://medium.com/@libertylocked/ec-signatures-and-recovery-in-ethereum-smart-contracts-560b6dd8876
  function ticketVerificationCode(uint24 _tokenId) public view returns(bytes32) {
    return d_data.ticketVerificationCode(_tokenId);
  }
  
  function isOwnerSig(uint24 _tokenId, bytes memory signature) public view returns(bool) {
    return d_data.isOwnerSig(_tokenId, signature);
  }

  function useTicket(uint24 _tokenId, bytes memory signature) public returns(bool) {
    require(isOwnerSig(_tokenId,signature), "Incorrect signature!");
    require(msg.sender == d_data.d_organizer);
    return d_data.useTicketUnsafe(_tokenId);
  }

  // function markDelete(bool mark_delete) public {
  //   require(msg.sender == d_data.d_organizer);
  //   d_data.d_mark_delete=mark_delete;
  // }

  function transferFromImpl(address _from, address payable _to, uint24 _token) private {
      d_data.transferFromImpl(_from, _to, _token);
      emit Transfer(_from,_to,_token);
      Universe u = Universe(d_data.d_admin);
      u.addUserEvent(address(this),_to);
  }

  function setImg(string calldata _imgSrc) public {
    imgSrc = _imgSrc;
  }

}