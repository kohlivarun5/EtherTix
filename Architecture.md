![https://g.gravizo.com/#state](https://g.gravizo.com/svg?%40startuml%3B%0A%0Aactor%20Creator%3B%0Aactor%20Organizer%3B%0Aactor%20User1%3B%0Aactor%20User2%3B%0A%0Aparticipant%20%22Universe%22%20as%20U%3B%0Aparticipant%20%22Event%20(ERC721)%22%20as%20E%3B%0A%0AOrganizer%20-%3E%20U%3A%20createEvent%3B%20%0Aactivate%20U%3B%0A%0AU%20-%3E%20E%20%3A%20constructor%3B%0AU%20-%3E%20U%3A%20Fees%3B%0Adeactivate%20U%3B%0A%0AUser1%20-%3E%20E%3A%20BuyTicket%3B%0Aactivate%20E%3B%0A%0AE%20-%3E%20E%3A%20TicketPrice%3B%0AE%20-%3E%20U%3A%20Commission%3B%0AE%20-%3E%20User1%3A%20Ticket%3B%0Adeactivate%20E%3B%0A%0AUser2%20-%3E%20E%3A%20BuyTicket%3B%0Aactivate%20E%3B%0AUser1%20-%3E%20E%3A%20Sell%3B%0AE%20-%3E%20E%3A%20Commission%3B%0Adeactivate%20E%3B%0A%0AOrganizer%20-%3E%20E%3A%20Withdraw%3B%0Aactivate%20E%3B%0AE%20-%3E%20Organizer%3A%20Fees%3B%0Adeactivate%20E%3B%0A%0ACreator%20-%3E%20U%3A%20Withdraw%3B%0Aactivate%20U%3B%0AU%20-%3E%20Creator%3A%20Fees%3B%0Adeactivate%20U%3B%0A%0AUser2%20-%3E%20E%3A%20UseTicket%3B%0Aactivate%20E%3B%0AE%20-%3E%20User2%3A%20QrCode%3B%0AUser2%20-%3E%20Organizer%3A%20QrCode%3B%0AOrganizer%20-%3E%20E%3A%20ConfirmUse%3B%0Adeactivate%20E%3B%0A%0A%40enduml)

```
@startuml;

actor Creator;
actor Organizer;
actor User1;
actor User2;

participant "Universe" as U;
participant "Event (ERC721)" as E;

Organizer -> U: createEvent; 
activate U;

U -> E : constructor;
U -> U: Fees;
deactivate U;

User1 -> E: BuyTicket;
activate E;

E -> E: TicketPrice;
E -> U: Commission;
E -> User1: Ticket;
deactivate E;

User2 -> E: BuyTicket;
activate E;
User1 -> E: Sell;
E -> E: Commission;
deactivate E;

Organizer -> E: Withdraw;
activate E;
E -> Organizer: Fees;
deactivate E;

Creator -> U: Withdraw;
activate U;
U -> Creator: Fees;
deactivate U;

User2 -> E: UseTicket;
activate E;
E -> User2: QrCode;
User2 -> Organizer: QrCode;
Organizer -> E: ConfirmUse;
deactivate E;

@enduml
```
