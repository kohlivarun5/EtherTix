let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x5D58C946544eAE3FD8f7a1e2A96E6c26e004c70A",
  address_uri
}