let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x71f82e6baf00ce382f8e39f4d0fe1c9259048191",
  address_uri
}
