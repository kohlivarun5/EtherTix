pragma solidity ^0.4.24;

contract Universe {
    
    address d_owner;
    
    constructor() public {
        d_owner = msg.sender;
    }
    
    function createEvent() public payable {
        
    }
    
    function getBalance() public constant returns(uint) {
        return address(this).balance;
    }
    
    function withdraw() public {
        require(msg.sender == d_owner);
        address(d_owner).transfer(getBalance());
    }
    
    
}
