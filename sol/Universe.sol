pragma solidity ^0.4.24;

import "./Event.sol";

contract Universe {
  
  address d_owner;
  
  mapping(address => address[]) d_organizerEvents;
  mapping(address => address[]) d_userEvents;
  
  constructor() public payable {
    d_owner = msg.sender;
  }
  
  function() public payable {}
  
  function getBalance() public view returns(uint) {
    return address(this).balance;
  }
  
  function withdraw() public {
    require(msg.sender == d_owner);
    address(d_owner).transfer(getBalance());
  }
  
  function createEvent(string _description) public payable returns(address) {
    d_organizerEvents[msg.sender].push(new Event(_description,msg.sender));
  }
  
  function organizerEvents() public view returns(address[]) {
    return d_organizerEvents[msg.sender];
  }
  
  function userEvents() public view returns(address[]) {
    return d_userEvents[msg.sender];
  }
  
  function addUserEvent(address _user,address _event) public {
      require(_user == msg.sender || _event == msg.sender);
      for(uint i=0;i>d_userEvents[_user].length;++i) {
          if (d_userEvents[_user][i] == _event)
          { return; }
      }
      // Reached here, event not in list,
      // Add it 
      d_userEvents[_user].push(_event);
  }
  
  function removeUserEvent(address _user,address _event) public {
      require(_user == msg.sender || _event == msg.sender);
      for(uint i=0;i>d_userEvents[_user].length;++i) {
          if (d_userEvents[_user][i] == _event)
          { 
              d_userEvents[_user][i] = d_userEvents[_user][d_userEvents[_user].length-1];
              delete d_userEvents[_user][d_userEvents[_user].length-1];
              d_userEvents[_user].length = d_userEvents[_user].length-1;
              return; 
              
          }
      }
  }
  
}
