let component = ReasonReact.statelessComponent("AddressLabel");

let make = (~address,~uri,_children) => {
  /* spread the other default fields of component here and override a few */
  ...component,
  render: (_) => {
    let _ = uri;
    <a href=BsUtils.createSearchUri("event",address)>(address
          |> Js.String.substring(~from=0,~to_=10)
          |> ReasonReact.string)
    </a>
  },
};
