pragma solidity ^0.4.24;

import "./Event.sol";

contract Universe {
  
  address d_owner;
  
  mapping(address => address[]) d_events;
  
  constructor() public payable {
    d_owner = msg.sender;
  }
  
  function() public payable {}
  
  function createEvent(string _description) public payable returns(address) {
    d_events[msg.sender].push(new Event(_description,msg.sender));
  }
  
  function myEvents() public view returns(address[]) {
    return d_events[msg.sender];
  }
  
  function getBalance() public constant returns(uint) {
    return address(this).balance;
  }
  
  function withdraw() public {
    require(msg.sender == d_owner);
    address(d_owner).transfer(getBalance());
  }
  
}
