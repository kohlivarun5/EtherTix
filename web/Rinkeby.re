let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x2424b922B9Fe59f799c9a6a274495Dd4c7245512",
  address_uri
}