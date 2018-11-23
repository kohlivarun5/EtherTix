let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xc587002fbb783d459b1a9f609d68439ba7242366",
  address_uri
}
