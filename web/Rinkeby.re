let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x7B110Daf7AE89Ae1602620afDDf66eB185937d34",
  address_uri
}
