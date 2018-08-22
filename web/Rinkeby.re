let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x8b9ae6d81b48050b0cc44fe49c05c11807d60c8e",
  address_uri
}
