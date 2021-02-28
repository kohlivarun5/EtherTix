pragma solidity >=0.6.0 <0.8.0;

import "./Event.sol";

contract Universe {
  
  address payable private d_owner;
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
  
  receive() external payable {}
  
  function getBalance() public view returns(uint) {
    return address(this).balance;
  }

  function isOwner() public view returns(bool) {
    return msg.sender == d_owner;
  }
  
  function withdraw() public {
    require(msg.sender == d_owner);
    d_owner.transfer(getBalance());
  }
  
  function createEvent(string memory _description) public payable returns(address) {
    Event eventAddr = new Event(_description,msg.sender,d_commission_percent);
    emit OrganizerEvents(address(eventAddr), msg.sender, true,_description);
    return address(eventAddr);
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

  function verifiedUser(address _user) public view returns(string memory) {
    return d_verified_users[_user];
  }

  function verifyUser(address _user,string memory info) public {
    require(msg.sender == d_owner);
    d_verified_users[_user] = info;
  }
  
}

