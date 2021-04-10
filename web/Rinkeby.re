let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x7fb6f6Da962692101921fD86D4e070C3C18d3b71",
  address_uri
}