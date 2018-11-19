let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x7c39a6f6c3bfc540a87957dc8585901d1224e4dd",
  address_uri
}
