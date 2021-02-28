let address_uri(address) =
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x3fb8C34b98Ff3327762c3d1Dc667a05d0BCbB3B5",
  address_uri
}
