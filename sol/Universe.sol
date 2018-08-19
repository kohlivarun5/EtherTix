pragma solidity ^0.4.24;

import "./Event.sol";

contract Universe {
  
  address d_owner;
  address internal d_userEvents;
  
  mapping(address => address[]) d_organizerEvents;
  
  constructor(address _userEvents) public payable {
    d_owner = msg.sender;
    d_userEvents = _userEvents;
  }
  
  function() public payable {}
  
  function createEvent(string _description) public payable returns(address) {
    d_organizerEvents[msg.sender].push(new Event(_description,msg.sender,d_userEvents));
  }
  
  function myEvents() public view returns(address[]) {
    return d_organizerEvents[msg.sender];
  }
  
  function getBalance() public view returns(uint) {
    return address(this).balance;
  }
  
  function withdraw() public {
    require(msg.sender == d_owner);
    address(d_owner).transfer(getBalance());
  }
  
}
