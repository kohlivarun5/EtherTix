let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x60360a79259ffd27cedb631075dcb9c32bf56dc9",
  address_uri
}
