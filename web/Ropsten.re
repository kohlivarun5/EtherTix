let address_uri(address) = 
  Js.String.concat(address,"https://ropsten.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xe8e2556ff6be12134e37b0971d45b8f0ed6e28c3",
  address_uri
}

