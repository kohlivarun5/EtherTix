let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x4f017720ed8efe514c3fa1d98bb29ba5a33db6cf",
  address_uri
}
