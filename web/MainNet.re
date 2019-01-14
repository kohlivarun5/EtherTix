let address_uri(address) = 
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xAD6505C205287594683B7C76aF80F8AA07558c91",
  address_uri
}
