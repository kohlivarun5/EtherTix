let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xc6f230578ea6738db80b80c052b49e3976429157",
  address_uri
}
