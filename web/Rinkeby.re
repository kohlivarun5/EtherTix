let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x2c835C1df1E44fb40d4a90B7EB3426a7e45c7878",
  address_uri
}
