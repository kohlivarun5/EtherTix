let address_uri(address) = 
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x288800b89Aca488185479cf66281F854B4930791",
  address_uri
}
