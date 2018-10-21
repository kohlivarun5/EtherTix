let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x303Dd22b4545b30d5666C0C947030201308467dc",
  address_uri
}
