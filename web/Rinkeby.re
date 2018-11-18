let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x480abeced18379a0584e72fc517788bbcb210e9e",
  address_uri
}
