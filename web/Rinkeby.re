let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x18fda9d4b130790e358f64e0a17f5488a0127ae5",
  address_uri
}
