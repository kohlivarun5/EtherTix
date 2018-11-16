let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xa57dc1da88d912d93b5178c1e1e600bd5f59534d",
  address_uri
}
