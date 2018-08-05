pragma solidity ^0.4.24;

import "./ERC721.sol";

contract Event is ERC721 {
 
  address internal d_creator;
  address internal d_organizer;
  
  string internal d_description;
  
  // Array with all token ids, used for enumeration
  uint256[] internal d_ticket_ids;
  
  // Mapping from owner to list of owned token IDs
  mapping(address => uint256[]) internal d_owner_tokens;
  
  // Mapping from token id to owner address
  mapping(uint256 => address) internal d_token_owner;
  
  // Mapping from token ID to approved address
  mapping (uint256 => address) internal d_token_a;
  

  constructor(string _description, uint256 _numTickets) public {
    d_description = _description;
    for (uint256 i=0;i<_numTickets;++i) {
      d_ticket_ids.push(i);
    }
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
    return _tokenId >= 0 && _tokenId < d_ticket_ids.length;
  }

  function approve(address _to, uint256 _tokenId) public;
  function getApproved(uint256 _tokenId)
    public view returns (address _operator);

  function setApprovalForAll(address _operator, bool _approved) public;
  function isApprovedForAll(address _owner, address _operator)
    public view returns (bool);

  function transferFrom(address _from, address _to, uint256 _tokenId) public;
  function safeTransferFrom(address _from, address _to, uint256 _tokenId)
    public;

  function safeTransferFrom(
    address _from,
    address _to,
    uint256 _tokenId,
    bytes _data
  )
    public;

  
  
  
}