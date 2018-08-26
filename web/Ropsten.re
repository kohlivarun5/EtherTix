let address_uri(address) = 
  Js.String.concat(address,"https://ropsten.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x8e28b14fe40ec398b37582c842618690b85bd9f1",
  address_uri
}

