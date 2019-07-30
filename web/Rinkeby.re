let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x478291fb43f18d31fa3a2222432107842e70d04f",
  address_uri
}
