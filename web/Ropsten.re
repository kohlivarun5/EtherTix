let address_uri(address) = 
  Js.String.concat(address,"https://ropsten.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x5b9e2d7ddc7bb47fe2139bb7bf941dff3625db95",
  address_uri
}

