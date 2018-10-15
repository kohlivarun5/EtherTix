let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x9CDE27cf36687A505aeA44965cB1cE425dd82902",
  address_uri
}
