let address_uri(address) = 
  Js.String.concat(address,"https://ropsten.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x6f142e94f8bf52664eb63ecdf292924d5e7046b3",
  address_uri
}

