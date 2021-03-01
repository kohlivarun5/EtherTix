let address_uri(address) =
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x8047f5aE251b032f7B6E8bc6329b4C28Abc0f353",
  address_uri
}
