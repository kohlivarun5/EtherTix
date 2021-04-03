let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xEbe476F56aBFa3cF1a59BD1754dbdD92C8e9e6e6",
  address_uri
}
