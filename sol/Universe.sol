pragma solidity ^0.4.24;

import "./Event.sol";

contract Universe {
  
  address private d_owner;
  uint256 d_commission_percent;

  mapping(address => string) d_verified_users;
  
  event OrganizerEvents(
        address indexed eventAddr,
        address indexed organizerAddr,
        bool    indexed active,
        string          description
  );
        
  event UserEvents(
        address indexed eventAddr,
        address indexed userAddr,
        bool    indexed active);
  
  constructor(uint256 commission_percent) public payable {
    d_owner = msg.sender;
    d_commission_percent = commission_percent;
  }
  
  function() public payable {}
  
  function getBalance() public constant returns(uint) {
    return address(this).balance;
  }

  function isOwner() public constant returns(bool) {
    return msg.sender == d_owner;
  }
  
  function withdraw() public {
    require(msg.sender == d_owner);
    address(d_owner).transfer(getBalance());
  }
  
  function createEvent(string _description) public payable returns(address) {
    address eventAddr = new Event(_description,msg.sender,d_commission_percent);
    emit OrganizerEvents(eventAddr, msg.sender, true,_description);
    return eventAddr;
  }
  
  function addUserEvent(address _event,address _user) public {
      emit UserEvents(_event,_user,true);
  }
  
  function removeUserEvent(address _event,address _user) public {
      emit UserEvents(_event,_user,false);
  }

  function updateCommission(uint256 commission_percent) public {
    require(msg.sender == d_owner,"Owner can update commission");
    d_commission_percent=commission_percent;
  }

  function verifiedUser(address _user) public constant returns(string) {
    return d_verified_users[_user];
  }

  function verifyUser(address _user,string info) public {
    require(msg.sender == d_owner);
    d_verified_users[_user] = info;
  }
  
}
