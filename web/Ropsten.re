let address_uri(address) = 
  Js.String.concat(address,"https://ropsten.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xfE090807e75bb4F029A7Bd217F766cB09b945b90",
  address_uri
}

